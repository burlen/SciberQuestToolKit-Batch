<?xml version="1.0"?>
<PoincareBatch>

  <vtkSQLog/>

  <vtkSQBOVMetaReader
      block_size="64 64 64"
      block_cache_ram_factor="0.5"
      periodic_bc="1 1 1">
       <arrays> b </arrays>
  </vtkSQBOVMetaReader>

  <vtkSQPlaneSource
      origin="0 511 0"
      point1="1023 511 0"
      point2="0 511 1023"
      resolution="32 32"
      immediate_mode="1"/>

  <vtkSQFieldTracer
      integrator_type="2"
      max_arc_length="200000"
      max_step_size="0.001"
      null_threshold="0.001"
      forward_only="0"/>

</PoincareBatch>
