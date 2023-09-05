# Changelog

Project Coordinators: Florian Dubath @fdubath

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

Note: the issue number refer to SDC-CH internal tracking.
## [1.8.4] 2023-09-05
### Fixed
 - Fix a regression in the grid comparison preventing the GUI to use newly generated MW grid. https://redmine.astro.unige.ch/issues/29732

## [1.8.3] 2023-09-04
### Fixed
 - remove Alexandria use of deprecated class preventing build on libcxx>=1.16, old libcxx version had to be removed as it was not allowing Conda compilation

## [1.8.0] 2023-06-08
### Fixed
- Fix the PP based on luminosity by providing the unreddedned luminosity https://redmine.astro.unige.ch/issues/29197
- Fix a bug in the refreshing of the parameter space page after saving a new or a re-nomed parameter sapace. https://redmine.astro.unige.ch/issues/29020
- Fix a bug in the GUI when saving a PP with Log part https://redmine.isdc.unige.ch/issues/28996

## [1.7.0] 
QT6 -  due to release constraints no proper release for now

## [1.6.2] 2023-04-26
### Changed
- Add the display of the Solar luminosity based SED scaling when displaying SED in "PhosphorosComputeModelSed" https://redmine.isdc.unige.ch/issues/28993
- Add a Phosphoros tool (FMG) to convert fits representation of the Model Grids to the Phosphoros format https://redmine.isdc.unige.ch/issues/28788

## [1.6.0] 2023-03-28
### Changed
- Add the possibility to define PP with $pp= A*L0 + B + C*LOG(D*L)$ (Where C and D param are new) The code reading files is backward compatible.  https://redmine.isdc.unige.ch/issues/28755
- Add a button which delete the IntermediateData grids (for saving disk space) https://redmine.isdc.unige.ch/issues/28796

### Fixed
- Refactor the GUI in order to add loading message and speed up the navigation between the pages by avoiding reloading the AuxiliaryData. https://redmine.isdc.unige.ch/issues/28808


## [1.5.3] 2023-02-22
### Changed
- Change the reference for Alexandria back to 2.28.0 (Elements 6.1.1) (For Euclid Pipeline's reason. This temporary reintroduce the bug 28352)

### Fixed
- Fix a bug in the Id columns format when the type is string. This change rises the memory footprint when sampling the results. Can be overcome by changing the chunk size or using an numeric (long) column for the OBJECT_ID https://redmine.isdc.unige.ch/issues/28519
- Fix a bug: ComputeRedshift executable was not creating the output file if empty. https://redmine.isdc.unige.ch/issues/28536



## [1.5.4] 2023-03-01
### Changed
- Change the reference for Alexandria back to 2.28.3 (Elements 6.1.2) This fix back the bug 28352

### Fixed
- Fix a bug in the sampling of the PDF. https://redmine.isdc.unige.ch/issues/28579

## [1.5.3] 2023-02-22
### Changed
- Change the reference for Alexandria back to 2.28.0 (Elements 6.1.1) (For Euclid Pipeline's reason. This temporary reintroduce the bug 28352)

### Fixed
- Fix a bug in the Id columns format when the type is string. This change rises the memory footprint when sampling the results. Can be overcome by changing the chunk size or using an numeric (long) column for the OBJECT_ID https://redmine.isdc.unige.ch/issues/28519
- Fix a bug: ComputeRedshift executable was not creating the output file if empty. https://redmine.isdc.unige.ch/issues/28536


## [1.5.2] 2023-01-27

### Fixed
- Fix a bug in Alexandria which was causing crash when only physical parameters with 1 char lenght name where defined. https://redmine.isdc.unige.ch/issues/28352

### Changed
- Change the reference for Alexandria 2.28.2 (Elements 6.1.2)

## [1.5.1] 2023-01-26

### Fixed
- Fix memory limit management in GUI. https://redmine.isdc.unige.ch/issues/28139
- Fix the way Phosphoros BRS clean the reference sample folder with option --reference-sample-overwrite: Do not delete file with other extension than .npy. https://redmine.isdc.unige.ch/issues/28172


## [1.5.0] 2022-11-29

### Changed
- Change the reference for Alexandria 2.28.0 (Elements 6.1.1)

## [1.4.0] 2022-11-02

### Added
- Lock the navigation and add a message while the app is reloading the DataPack. https://redmine.isdc.unige.ch/issues/27412

### Changed
- Change the format of the Changelog
- Speedup the opening of the app by removing unecessary checks
- SED selector dialog starts with collapsed group (instead of expended). https://redmine.isdc.unige.ch/issues/27838

### Fixed
- Fix slicing of the model grid when we have less filters available that modelled. https://redmine.isdc.unige.ch/issues/27364
- Add a message when 2 Dataset files conflict. https://redmine.isdc.unige.ch/issues/27413
- Fix PhosphorosPlotPhotometryComparison executable by adding missing arguments in the config. https://redmine.isdc.unige.ch/issues/27871
- Fix the test over missing plack map when computing the Zero point correction without this option. https://redmine.isdc.unige.ch/issues/27872

## [1.3.1] 2022-08-16

### Added
- Add this changelog. https://redmine.isdc.unige.ch/issues/26507
- Filters have been added to the Auxiliary Data. https://redmine.isdc.unige.ch/issues/21756
- Add the BuildPhotometry script to Phosphoros (as BPY). https://redmine.isdc.unige.ch/issues/26617
- Output the corrected photometry. https://redmine.isdc.unige.ch/issues/26822
- Compute 1D and 2D PDF for PP, add the sampling of the PP and units into the header of the fits. https://redmine.isdc.unige.ch/issues/26858

### Changed
- Update the default values in the compute zero-point correction popup. Reported in https://redmine.isdc.unige.ch/issues/26634
- Change the warning message displayed when one compute zero-point corrections with a photometric band having only missing values. https://redmine.isdc.unige.ch/issues/26634
- Update the default output folder name when the input catalog contains '.'. https://redmine.isdc.unige.ch/issues/26646
- Change the way the PP are extracted from the SEDs to speed  up the interface. https://redmine.isdc.unige.ch/issues/26860
- Change the default value of the luminosity sampling into the GUI. https://redmine.isdc.unige.ch/issues/26995
    
### Fixed
- Downloading of the Auxiliary Data has been fixed. https://redmine.isdc.unige.ch/issues/26654
- Typos and wording have been fixed/updated. https://redmine.isdc.unige.ch/issues/26406
- Locales using ',' as decimal separator are no longer a problem for saving the configuration. https://redmine.isdc.unige.ch/issues/26411
- Fix the way the grid files are named. This should fix a  bunch of issues (to be confirmed individually)
    - https://redmine.isdc.unige.ch/issues/26447
    - https://redmine.isdc.unige.ch/issues/26504
    - https://redmine.isdc.unige.ch/issues/26517
- Remove check on the galactic correction grids preventing run with unselected filters. https://redmine.isdc.unige.ch/issues/26479
- Ensure that the width of the OBJECT_ID column in sampling files is large enough (the automatic case was trunkating the ID in some cases). https://redmine.isdc.unige.ch/issues/26751
- Following up issue 26751, which was fixed temporary, we get the type of the OBJECTD_ID column in the sampling and index files from the type found in the input catalog file. https://redmine.isdc.unige.ch/issues/26756
- Fix the configuration produced for the CPC. Reported in https://redmine.isdc.unige.ch/issues/26640
- Fix the PP unit clash. https://redmine.isdc.unige.ch/issues/26822
- Fix the computation of the Zero point corrections for photometries flagged as missing or upper limit. https://redmine.isdc.unige.ch/issues/26640

