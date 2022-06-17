
#include<algorithm>
#include <QFileDialog>
#include <QFileInfo>
#include <QProcess>
#include <QThread>
#include <QStringList>
#include <QScrollBar>
#include <iostream>
#include <fstream>
#include <QMessageBox>
#include "FileUtils.h"
#include <sstream>
#include <QList>
#include <QStandardItem>
#include <QStandardItemModel>
#include "ElementsKernel/Logging.h"

#include <boost/filesystem.hpp>
#include "PhzUITools/CatalogColumnReader.h"
#include "PhzQtUI/NumberItemDelegate.h"
#include "PhzQtUI/IntItemDelegate.h"
#include "PhzQtUI/BoolItemDelegate.h"

#include "PhzQtUI/DialogPpPdf.h"
#include "ui_DialogPpPdf.h"
#include <QApplication>

using namespace std;

namespace Euclid {
namespace PhzQtUI {

static Elements::Logging logger = Elements::Logging::getLogger("DialogPpPdf");

DialogPpPdf::DialogPpPdf(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogPpPdf){
	ui->setupUi(this);

	ui->wg_1d->hide();
	ui->wg_2d->hide();
}

DialogPpPdf::~DialogPpPdf(){};


void DialogPpPdf::setFolder(std::string result_folder) {
	m_result_folder = result_folder;
	ui->out_cons->setPlainText("");
	QString prog =  QString::fromStdString("PhosphorosExtractPpPdf");
	QStringList arguments;
	arguments << "-z" << "true" << "--result-dir" << QString::fromStdString(m_result_folder)
	          << "--output-range-file" << QString::fromStdString(m_result_folder + "/pp_ranges.csv");

    logger.debug() << prog.toStdString() << " " << arguments.join(" ").toStdString();
	m_P =new QProcess(this);
	m_P->setProcessChannelMode(QProcess::MergedChannels);
	connect(m_P, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processingFinished(int, QProcess::ExitStatus)));
	m_processing=true;
	m_P->start(prog, arguments);
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(updateOutCons()));
	m_timer->start(100);
}

void DialogPpPdf::updateOutCons() {
	QString result_all = m_P->readAllStandardOutput();
	ui->out_cons->setPlainText(ui->out_cons->toPlainText() + result_all);
	ui->out_cons->verticalScrollBar()->setValue(ui->out_cons->verticalScrollBar()->maximum());
}

void DialogPpPdf::processingFinished(int, QProcess::ExitStatus){
	 m_timer->stop();
     disconnect(m_timer,SIGNAL(timeout()),0,0);
     disconnect(m_P,SIGNAL(finished(int, QProcess::ExitStatus)),0,0);
	 m_processing=false;
	 if (m_configured) {
		QMessageBox::information(this, tr("Completed"), tr("The computation of the PP's PDF has been completed!"),
		 				                   QMessageBox::Ok, QMessageBox::Ok);
		this->accept();
	} else {
		ui->out_cons->setPlainText("");
		m_configured= true;
		m_pps = std::vector<std::string>{};
		std::vector<double> min_val{};
		std::vector<double> max_val{};
		string row;
		ifstream readFile(m_result_folder + "/pp_ranges.csv");
		bool first_row = true;
		while (getline (readFile, row)) {
			if (first_row) {
				first_row=false;
			} else {
				auto parts = QString::fromStdString(row).split(QString(","));
				m_pps.push_back(parts[0].toStdString());
				min_val.push_back(parts[1].toFloat());
				max_val.push_back(parts[2].toFloat());
			}
		}
		readFile.close();

		QStandardItemModel* grid_model = new QStandardItemModel();
	    grid_model->setColumnCount(5);
	    QStringList  setHeaders;
	    setHeaders<<"Name"<<"Min"<<"Max"<<"Sample #"<<"1D PDF";
	    grid_model->setHorizontalHeaderLabels(setHeaders);
	    for (size_t index=0; index < m_pps.size(); ++index) {
	    	QList<QStandardItem*> items;
	    	QStandardItem* item_id = new QStandardItem(QString::fromStdString(m_pps[index]));
	    	item_id->setFlags(Qt::NoItemFlags);
	    	items.push_back(item_id);
	    	QStandardItem* item_min = new QStandardItem(QString::number(min_val[index]));
	    	items.push_back(item_min);
	    	QStandardItem* item_max = new QStandardItem(QString::number(max_val[index]));
	    	items.push_back(item_max);
	    	QStandardItem* item_sample = new QStandardItem(QString::number(50));
	    	items.push_back(item_sample);

	    	if (index == m_pps.size() - 1) {
	    		QStandardItem* item_pdf = new QStandardItem(QString::number(0));
	    		item_pdf->setFlags(Qt::NoItemFlags);
	    		items.push_back(item_pdf);
	    	} else {
	    		QStandardItem* item_pdf = new QStandardItem(QString::number(1));
	    		items.push_back(item_pdf);
	    	}


	    	grid_model->appendRow(items);

	    }
		ui->tv_range->setModel(grid_model);
        ui->tv_range->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
		ui->tv_range->setItemDelegateForColumn(1, new NumberItemDelegate());
		ui->tv_range->setItemDelegateForColumn(2, new NumberItemDelegate());
		ui->tv_range->setItemDelegateForColumn(3, new IntItemDelegate(1,5000));
	    ui->tv_range->setItemDelegateForColumn(4, new BoolItemDelegate());
		ui->wg_1d->show();

		//---------------------------------------------------------------

		QStandardItemModel* grid_model_2d = new QStandardItemModel();
		grid_model_2d->setColumnCount(m_pps.size());
		QStringList  setHeaders_2d;
		setHeaders_2d<<"Name";
		for (size_t index=1; index < m_pps.size(); ++index){
			setHeaders_2d<<QString::fromStdString(m_pps[index]);
		}

		grid_model_2d->setHorizontalHeaderLabels(setHeaders_2d);

		for (size_t index=0; index < m_pps.size()-1; ++index) {
			QList<QStandardItem*> items;
			QStandardItem* item_id = new QStandardItem(QString::fromStdString(m_pps[index]));
			item_id->setFlags(Qt::NoItemFlags);
			items.push_back(item_id);

			for (size_t index_1=1; index_1 < m_pps.size(); ++index_1) {
				if (index_1<=index){
					QStandardItem* item = new QStandardItem(QString::fromStdString(""));
					item->setFlags(Qt::NoItemFlags);
					items.push_back(item);
				} else {
					QStandardItem* item_pdf = new QStandardItem(QString::number(0));
					items.push_back(item_pdf);
				}
			}
			grid_model_2d->appendRow(items);
		}

		ui->tv_2d->setModel(grid_model_2d);
        ui->tv_2d->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
		for (size_t index=1; index < m_pps.size(); ++index){
			ui->tv_2d->setItemDelegateForColumn(index, new BoolItemDelegate());
		}

		ui->wg_2d->show();
	}
}

void DialogPpPdf::on_btn_cancel_clicked() {
	if (m_processing) {
		m_P->kill();
		m_timer->stop();
	    disconnect(m_timer,SIGNAL(timeout()),0,0);
	    disconnect(m_P,SIGNAL(finished(int, QProcess::ExitStatus)),0,0);
		m_processing=false;
	}
	accept();
}

void DialogPpPdf::on_btn_save_clicked() {
	// collect pdf 1d
	std::vector<std::string> pdf_1d {};
	std::vector<double> min_v {};
	std::vector<double> max_v {};
	std::vector<int> number {};
	for (size_t index=0; index < m_pps.size(); ++index) {
		if (ui->tv_range->model()->data(ui->tv_range->model()->index(index, 4)).toInt()==1) {
			pdf_1d.push_back(m_pps[index]);
	    }
		min_v.push_back(ui->tv_range->model()->data(ui->tv_range->model()->index(index, 1)).toFloat());
		max_v.push_back(ui->tv_range->model()->data(ui->tv_range->model()->index(index, 2)).toFloat());
		number.push_back(std::max(1,ui->tv_range->model()->data(ui->tv_range->model()->index(index, 3)).toInt()));
	}


	// collect pdf 2d
	std::vector<std::string> pdf_2d {};
	for (size_t index=0; index < m_pps.size()-1; ++index) {
		for (size_t index_1=1; index_1 < m_pps.size(); ++index_1) {
			if (ui->tv_2d->model()->data(ui->tv_2d->model()->index(index, index_1)).toInt()==1) {
				std::string name_2d = m_pps[index] + "_" + m_pps[index_1];
				pdf_2d.push_back(name_2d);
			}
		}
	}

	// Create the command
	if (pdf_1d.size() + pdf_2d.size()==0) {
		QMessageBox::warning(this, tr("No PDF selected"), tr("Please select at least one PDF to be computed!"),
				                   QMessageBox::Cancel, QMessageBox::Cancel);
	} else {
		std::string bins="";
		std::string ranges="";
		for (size_t index=0; index < m_pps.size(); ++index) {
			if (index>0){
				bins += ",";
				ranges += ",";
			}
			bins += m_pps[index] + ":" + std::to_string(number[index]);
			ranges += m_pps[index] + ":" + std::to_string(min_v[index]) + ":" + std::to_string(max_v[index]);
		}

		std::string pdfs="";
		for (size_t index=0; index < pdf_1d.size(); ++index) {
			if (index>0){
				pdfs += ",";
			}
			pdfs += pdf_1d[index];
		}

		for (size_t index=0; index < pdf_2d.size(); ++index) {
			if (pdfs.length()>0){
				pdfs += ",";
			}
			pdfs += pdf_2d[index];
		}

		QString prog =  QString::fromStdString("PhosphorosExtractPpPdf");
		QStringList arguments;
		arguments << "--result-dir" << QString::fromStdString(m_result_folder)
		          << "--pdf-bin" << QString::fromStdString(bins)
				  << "--pdf-range" << QString::fromStdString(ranges)
				  << "--pdf-list" << QString::fromStdString(pdfs)
				  << "--output-pdf-file" <<  QString::fromStdString(m_result_folder + "/pp_pdf.fits");

	    logger.debug() << prog.toStdString() << " " << arguments.join(" ").toStdString();

		m_P =new QProcess(this);
		m_P->setProcessChannelMode(QProcess::MergedChannels);
		connect(m_P, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processingFinished(int, QProcess::ExitStatus)));
		m_processing=true;
		m_P->start(prog, arguments);
		m_timer = new QTimer(this);
		connect(m_timer, SIGNAL(timeout()), this, SLOT(updateOutCons()));
		m_timer->start(100);

	}




}



}
}
