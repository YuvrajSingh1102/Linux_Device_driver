#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0x64491f21, "module_layout" },
	{ 0x1dbd1ee9, "class_destroy" },
	{ 0x1414c822, "platform_driver_unregister" },
	{ 0xcaf7d923, "__platform_driver_register" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x303b348c, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x69512d2b, "device_create" },
	{ 0x460ec521, "cdev_add" },
	{ 0xe7c2917d, "cdev_init" },
	{ 0xeff31b63, "devm_kmalloc" },
	{ 0x41f74b9c, "cdev_del" },
	{ 0xd6de966e, "device_destroy" },
	{ 0xc5850110, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "887AB772F36F0B5AD10F3DC");
