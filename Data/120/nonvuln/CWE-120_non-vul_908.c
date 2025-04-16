static void __exit si4713_module_exit(void)
{
	i2c_del_driver(&si4713_i2c_driver);
}