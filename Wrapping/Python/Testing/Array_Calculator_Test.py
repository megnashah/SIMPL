# Pipeline : 
# This pipeline is designed to test the Attribute Array Calculator filter
#

from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd


def array_calculator_test():
    # Create Data Container Array using simpl directly
    dca = simpl.DataContainerArray.New()

    # Create a Data Container using the pythonic version
    err = simplpy.create_data_container(dca, "ImageDataContainer")
    assert err == 0

    # Create an Attribute Matrix
    amDims = [[4, 5, 6]]
    tableData = sc.CreateDynamicTableData(amDims)
    dap = simpl.DataArrayPath("ImageDataContainer", "CellAttributeMatrix", "")
    err = simplpy.create_attribute_matrix(dca, dap, simpl.AttributeMatrix.Type.Cell, tableData)
    assert err == 0
    
    # Add a data array to the Attribute Matrix by using a Filter
    dap = simpl.DataArrayPath("ImageDataContainer", "CellAttributeMatrix", "Int32Data")
    inputRange = (0,0)
    err = simplpy.create_data_array(dca,simpl.ScalarTypes.Int32, 1, dap, 0, "37", inputRange)
    assert err == 0
    
    # Test: Attribute Array Calculator
    err = simplpy.array_calculator(dca, simpl.DataArrayPath("ImageDataContainer", "CellAttributeMatrix", ""), 
                                   "Int32Data*3.1415927*100", simpl.DataArrayPath("ImageDataContainer", "CellAttributeMatrix", "Output"),
                                   simpl.AngleUnits.Radians, simpl.ScalarTypes.UInt32)
    assert err == 0


    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory()
                              + "/Data/Output/CoreFilterTests/ArrayCalculatorTest.dream3d", dca)
    assert err == 0


"""
Main entry point for python script
"""
if __name__ == "__main__":
    array_calculator_test()
