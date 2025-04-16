static int __init si4713_module_init(void)
{
	return i2c_add_driver(&si4713_i2c_driver);
}