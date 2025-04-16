static u32 ismt_func(struct i2c_adapter *adap)
{
	return I2C_FUNC_SMBUS_QUICK		|
	       I2C_FUNC_SMBUS_BYTE		|
	       I2C_FUNC_SMBUS_BYTE_DATA		|
	       I2C_FUNC_SMBUS_WORD_DATA		|
	       I2C_FUNC_SMBUS_PROC_CALL		|
	       I2C_FUNC_SMBUS_BLOCK_PROC_CALL	|
	       I2C_FUNC_SMBUS_BLOCK_DATA	|
	       I2C_FUNC_SMBUS_I2C_BLOCK		|
	       I2C_FUNC_SMBUS_PEC;
}