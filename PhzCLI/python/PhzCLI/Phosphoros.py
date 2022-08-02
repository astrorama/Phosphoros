from __future__ import print_function
from subprocess import call
import sys
import os
import ElementsKernel.Configuration as el_conf

class PhosphorosAction:
    def __init__(self, names, executable, help=''):
        self.names = names
        self.executable = executable
        self.help = help

action_list = [
    PhosphorosAction(['help', 'h', '-h', '--help'], 'help',
                     'Shows this help message'),
    PhosphorosAction(['version', '-v', '--version'], 'version',
                     'Shows the version of Phosphoros'),
    PhosphorosAction(['dump_config', 'DC'], 'dump_config',
                     'Print an example configuration file for the given command'),
    PhosphorosAction(['GUI'], 'PhosphorosUI',
                     'Starts the Graphical User Interface'),
    PhosphorosAction(['compute_model_grid', 'CMG'], 'PhosphorosComputeModelGrid',
                     'Computes a grid containing the model photometries'),
    PhosphorosAction(['compute_galactic_correction_coeff_grid', 'CGCCG'], 'PhosphorosComputeGalacticAbsorptionCoefficientGrid',
                     'Computes a grid containing the coefficient for the correction of the galactic absorption'),
    PhosphorosAction(['compute_filter_variation_coeff_grid', 'CFVCG'], 'PhosphorosComputeFilterVariationCoefficientGrid',
                     'Computes a grid containing the coefficient for the correction of the filters variation'),
    PhosphorosAction(['compute_photometric_corrections', 'CPC'], 'PhosphorosComputePhotometricCorrections',
                     'Calculates the Photometric Zero Point Corrections'),
    PhosphorosAction(['compute_sed_weights', 'CSW'], 'PhosphorosComputeSedWeight',
                     'Calculate the weights of the SEDs and produce a prior from it'),
    PhosphorosAction(['compute_redshifts', 'CR'], 'PhosphorosComputeRedshifts',
                     'Calculates the PHZ results for a given catalog'),
    PhosphorosAction(['display_filters', 'DF'], 'PhosphorosLsAux --type Filters',
                     'Browses the available filter transmission'),
    PhosphorosAction(['display_seds', 'DS'], 'PhosphorosLsAux --type SEDs',
                     'Browses the available SED templates'),
    PhosphorosAction(['display_reddening_curves', 'DRC'], 'PhosphorosLsAux --type ReddeningCurves',
                     'Browses the available reddening curves'),
    PhosphorosAction(['display_model_grid', 'DMG'], 'PhosphorosDisplayModelGrid',
                     'Browses through a grid containing model photometries'),
    PhosphorosAction(['compute_model_sed', 'CMS'], 'PhosphorosComputeModelSed',
                     'Displays the reddened, redshifted SED data'),
    PhosphorosAction(['plot_specz_comparison', 'PSC'], 'PhosphorosPlotSpecZComparison',
                     'Shows plots comparing the PHZ result with SPECZ'),
    PhosphorosAction(['plot_stacked_pdz', 'PSP'], 'PlotStackedPdfPitAndCrps',
                     'Shows plots comparing the PHZ result with SPECZ'),
    PhosphorosAction(['plot_flux_model_diff', 'PFM'], 'PhosphorosPlotFluxDiff',
                     'Shows plots comparing the measured flux with the flux from the best fitted model'),
    PhosphorosAction(['plot_posterior', 'PP'], 'PhosphorosPlotPosterior',
                     'Plots views of a multi-dimensional posterior'),
    PhosphorosAction(['plot_photometry_comparison', 'PPC'], 'PhosphorosPlotPhotometryComparison',
                     'Plots the best fitted model and observed photometries'),
    PhosphorosAction(['order_seds', 'OS'], 'PhosphorosOrderSeds',
                     'Orders SEDs and put them into the <order.txt> file'),
    PhosphorosAction(['create_flat_grid_prior', 'CFGP'], 'CreateFlatGridPrior',
                     'Creates a generic grid prior FITS file with all cells set to one, based on a model grid'),
    PhosphorosAction(['add_emission_lines', 'AEL'], 'PhosphorosAddEmissionLines',
                     'Adds emission lines to a set of SED templates'),
    PhosphorosAction(['build_reference_sample', 'BRS'], 'PhosphorosBuildReferenceSample',
                     'Builds an NNPZ reference sample from the results of a Phosphoros output catalog'),
    PhosphorosAction(['build_photometry', 'BPY'], 'PhosphorosBuildPhotometry',
                     'Build The photometry using the best fitted models from the results of a Phosphoros output catalog'),
    PhosphorosAction(['add_galactic_dust_data', 'AGDD'], 'AddGalDustToCatalog',
                     'Lookup galactical E(B-V) in the Planck dust map and add it as an additional catalog column'),
    PhosphorosAction(['process_output_pdz', 'POP'], 'ProcessPDF',
                     'Process PDZ by extracting the Mean, Confidence intervals and information about the first 2 modes.'),
    PhosphorosAction(['update_data_package', 'UDP'], 'UpdateDataPack',
                     'Check for Data Pacakge online.'),
    PhosphorosAction(['build_pp_config', 'BPPC'], 'PhosphorosBuildPPConfig',
                     'Build the Physical Parameter config allowing the redshift computation to output them.'),
    PhosphorosAction(['interpolate_seds', 'IS'], 'InterpolateSED',
                     'Create SEDs interpolated between existing SEDs.'),
    PhosphorosAction(['extract_pp_pdf', 'EPP'], 'PhosphorosExtractPpPdf',
                     'Extract 1d/2d pp pdf from the sampling of the full pdf.'),
    
]

def printHelp():
    print ('')
    print ('Usage: Phosphoros <action> <action_parameters>')
    print ('')
    print ('Available actions:')
    for action in action_list:
        message = action.names[0]
        for alter in action.names[1:]:
            message += ', ' + alter
        message += os.linesep + '    ' + action.help
        print (message)
    print ('')
    exit()

def printVersion():
    import PHOSPHOROS_VERSION
    print(PHOSPHOROS_VERSION.PHOSPHOROS_VERSION_STRING)
    exit()

def dumpConfig():
    if len(sys.argv) != 3 or sys.argv[2] in ('help', '--help', '-h'):
        print ('Usage: Phosphoros dump_config <COMMAND>')
        exit(1)

    # Get the action from the action_list
    cmd = sys.argv[2]
    action = [a for a in action_list if cmd in a.names]
    if len(action) == 0:
        print ('Unknown Phosphoros action:', cmd)
        exit(1)
    
    # Get the config file name from the executable name
    name = action[0].executable.split()[0] + '.conf'

    # Find the configuration file
    conf_file = [os.path.join(r,f) for conf_dir in reversed(el_conf.getConfigurationLocations()) for r,d,fs in os.walk(conf_dir) for f in fs if f == name]
    if len(conf_file) == 0:
        print ('Command', cmd, 'does not accept a configuration file')
        exit()

    with open(conf_file[0]) as f:
        print(f.read())
    exit()

if len(sys.argv) < 2:
    printHelp()

executable = [word for action in action_list if sys.argv[1] in action.names for word in action.executable.split()]
if not executable:
    print ('Unknown action: ' + sys.argv[1])
    print ('Try running "Phosphoros help"')
    exit(1)

if executable[0] == 'help':
    printHelp()

if executable[0] == 'version':
    printVersion()

if executable[0] == 'dump_config':
    dumpConfig()

phosphoros_conf = os.getenv('PHOSPHOROS_ROOT', os.path.expanduser('~')+os.sep+'Phosphoros') + os.sep + 'config'
if os.environ.get('ELEMENTS_CONF_PATH'):
    os.environ['ELEMENTS_CONF_PATH'] = phosphoros_conf + os.path.pathsep + os.environ['ELEMENTS_CONF_PATH']
else:
    os.environ['ELEMENTS_CONF_PATH'] = phosphoros_conf

cmd = executable + sys.argv[2:]
exit(call(cmd))
