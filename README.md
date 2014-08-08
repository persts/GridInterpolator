GridInterpolator
================  
The Grid interpolator simply applies a linear interpolation between two grids on a cell by cell basis.

InterpolationIncrement i,j = EndYear i,j - StartYear i,j / ( EndYear i,j - StartYear i,j )

For each year between the StartYear and EndYear  
CurrentYear i,j = StartYear i,j + ( InterpolationIncrement i,j * ( CurrentYear - StartYear ))

Raster file name format: _speciesname_year.extension_  
Example: Aucanc_2007.asc; Aucanc_2020.asc; Aucnac_2050.asc

Alpha Testing: Thanks to Kevin Koy and the participants of the "Modeling species extinctions risk under
climate change" sponsored by the EC FP6 MASCIS project for helping with the initial testing of this
application.

_This material is based in part upon work supported in part by the National Science Foundation under Grant
Number ARC-0732948 and NASA grant number NNX09AK19G. Any opinions, findings, and conclusions
or recommendations expressed in this material are those of the author(s) and do not necessarily reflect
the views of the National Science Foundation or NASA._
