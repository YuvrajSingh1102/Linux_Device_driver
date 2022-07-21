#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>
#include<linux/platform_device.h>
#include<linux/slab.h>
#include<linux/mod_devicetable.h>

#include "platform.h"

#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt,__func__

struct device_config
{
        int config_item1;
        int config_item2;
};

enum pcdev_names
{
        PCDEVA1X,
        PCDEVB1X,
        PCDEVC1X,
        PCDEVD1X
};

struct device_config pcdev_config[] =
{
        [PCDEVA1X] = {
                .config_item1 = 60,
                .config_item2 = 21
        },

        [PCDEVA1X] = {
                .config_item1 = 50,
                .config_item2 = 22
        },

        [PCDEVA1X] = {
                .config_item1 = 40,
                .config_item2 = 23
        },

        [PCDEVA1X] = {
                .config_item1 = 30,
                .config_item2 = 24
        }
};

struct platform_device_id pcdevs_id[] = 
{
	[0] = {
		.name = "pcdev-A1x",
		.driver_data = PCDEVA1X
	},

	[1] = {
		.name = "pcdev-B1x",
		.driver_data = PCDEVB1X
	},

	[2] = {
		.name = "pcdev-C1x",
		.driver_data = PCDEVC1X
	},

	[3] = {
		.name = "pcdev-D1x",
		.driver_data = PCDEVD1X
	}
};

/* Device private data structure */
struct pcdev_private_data
{
	struct pcdev_platform_data pdata;
	char *buffer;
	dev_t dev_num;
	struct cdev cdev;
};

/* Driver private data structure */
struct pcdrv_private_data
{
	int total_devices;
	dev_t device_num_base;
	struct class *class_pcd;
	struct device *device_pcd;
};

struct pcdrv_private_data pcdrv_data;

int check_permission(int dev_perm, int acc_mode) 
{
	if(dev_perm == RDWR) 
		return 0;
	if((dev_perm == RDONLY) && ((acc_mode & FMODE_READ) && !(acc_mode & FMODE_WRITE)))
		return 0;
	if((dev_perm == WRONLY) && ((acc_mode & FMODE_WRITE) && !(acc_mode & FMODE_READ)))
		return 0;

	return -EPERM;
}

loff_t pcd_lseek (struct file *filp, loff_t offset, int whence)
{
	return 0;
}

ssize_t pcd_read (struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
	return 0;
}

ssize_t pcd_write (struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
	return -ENOMEM;
}


int pcd_open (struct inode *inode, struct file *filp)
{
	return 0;
}

int pcd_release (struct inode *inode, struct file *filp)
{
	pr_info("Release was successful");
	return 0;
}

/* File operations of the driver */
struct file_operations pcd_fops = 
{
	.open = pcd_open,
	.write = pcd_write,
	.read = pcd_read,
	.llseek = pcd_lseek,
	.release = pcd_release,
	.owner = THIS_MODULE
};

/* Gets called when the device is removed from the system */
int pcd_platform_driver_remove(struct platform_device *pdev)
{
	struct pcdev_private_data *dev_data = dev_get_drvdata(&pdev->dev);

	/*1. Remove a device that was created with device_create() */
	device_destroy(pcdrv_data.class_pcd,dev_data->dev_num);

	
	/*2. Remove a cdev entry from the system */
	cdev_del(&dev_data->cdev);

	pcdrv_data.total_devices--;
		
	pr_info("A device is removed\n");
	return 0;
}

/* Gets called when the matched platform device is found */
int pcd_platform_driver_probe(struct platform_device *pdev)
{
	int ret;

	struct pcdev_private_data *dev_data;
	struct pcdev_platform_data *pdata;
	
	pr_info("A device was detected\n");

	/*1. Get the platfrom data */
	pdata = (struct pcdev_platform_data*)dev_get_platdata(&pdev->dev);
	if(!pdata) {
		pr_info("No platform data available\n");
		return -EINVAL;
	}
	
	/*2. Dynamically allocate memory for the device private data */
	dev_data = devm_kzalloc(&pdev->dev, sizeof(*dev_data), GFP_KERNEL);
	if(!dev_data) {
		pr_info("Cannot allocate memory\n");
		return -ENOMEM;
	}
	
	/* Save the device private data pointer in platform device structure */
	dev_set_drvdata(&pdev->dev, dev_data);

	dev_data->pdata.size = pdata->size;
	dev_data->pdata.perm = pdata->perm;
	dev_data->pdata.serial_number = pdata->serial_number;
	
	pr_info("Device serial number = %s\n", dev_data->pdata.serial_number);
	pr_info("Device size = %d\n", dev_data->pdata.size);
	pr_info("Device permissions = %d\n", dev_data->pdata.perm);
	
	pr_info("Config item 1 = %d\n", pcdev_config[pdev->id_entry->driver_data].config_item1);
	pr_info("Config item 2 = %d\n", pcdev_config[pdev->id_entry->driver_data].config_item2);

	/*3. Dynamically allocate memory for the device buffer using size informationfrom the platfrom data*/
	dev_data->buffer = devm_kzalloc(&pdev->dev, sizeof(dev_data->pdata.size),GFP_KERNEL);
	if(!dev_data->buffer) {
		pr_info("Cannot allocate memory\n");
		return -ENOMEM;
	}

	/*4. Get the device number */
	dev_data->dev_num = pcdrv_data.device_num_base + pdev->id; 
	
	/*5. Do cdev init and cdev add */
	cdev_init(&dev_data->cdev, &pcd_fops);

	dev_data->cdev.owner = THIS_MODULE;
	ret = cdev_add(&dev_data->cdev, dev_data->dev_num, 1);
	if(ret < 0) {
		pr_err("Cdev add failure\n");
		return ret;
	}
	/*6. Create device file for the detected platform device */
	pcdrv_data.device_pcd = device_create(pcdrv_data.class_pcd, NULL, dev_data->dev_num, NULL, "pcdev-%d", pdev->id);
	if(IS_ERR(pcdrv_data.device_pcd)){
		pr_err("Device create failed\n");
		ret = PTR_ERR(pcdrv_data.device_pcd);
		cdev_del(&dev_data->cdev);
		return ret;
	}

	pcdrv_data.total_devices++;

	pr_info("Probe was sucessful\n");

	return 0;
}

struct platform_device_id pcdevs_ids[] = 
{
	[0] = {
		.name = "pcdev-A1x"
	},	
	
	[1] = {
		.name = "pcdev-B1x"
	},	
	
	[2] = {
		.name = "pcdev-C1x"
	},	
	
	[3] = {
		.name = "pcdev-D1x"
	}
};

struct platform_driver pcd_platform_driver = 
{
	.probe = pcd_platform_driver_probe,
	.remove = pcd_platform_driver_remove,
	.id_table = pcdevs_ids,
	.driver = {
		.name = "pseudo-char-device"
	}
};

#define MAX_DEVICES 10

static int __init pcd_platform_driver_init(void) 
{
	int ret;

	/*1. Dynamically allocate a device number for MAX_DEVICES */
	ret = alloc_chrdev_region(&pcdrv_data.device_num_base,0,MAX_DEVICES,"pcdevs");
	if(ret < 0){
		pr_err("Alloc chrdev failed\n");
		return ret;
	}

	/*2. Create device class under /sys/class */
	pcdrv_data.class_pcd = class_create(THIS_MODULE, "pcd_class");
	if(IS_ERR(pcdrv_data.class_pcd)) {
		pr_err("Class creation failed\n");
		ret = PTR_ERR(pcdrv_data.class_pcd);
		unregister_chrdev_region(pcdrv_data.device_num_base, MAX_DEVICES);
		return ret;
	}

	pr_err("This is called.\n");
	/*3. Register a platform driver */
	platform_driver_register(&pcd_platform_driver);
	
	pr_info("Pcd platform driver loaded\n");
	return 0;
}

static void __exit pcd_platform_driver_cleanup(void)
{
	/* Unregister the platform driver */
	platform_driver_unregister(&pcd_platform_driver);

	/* Class destroy */
	class_destroy(pcdrv_data.class_pcd);
	
	/* Unregister the device member for MAX_DEVICES */
	unregister_chrdev_region(pcdrv_data.device_num_base, MAX_DEVICES);

	pr_info("Pcd platform driver unloaded\n");
}

module_init(pcd_platform_driver_init);
module_exit(pcd_platform_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YUVRAJ");
MODULE_DESCRIPTION("A pseudo character platform driver which handles n devices platform pcdevs");
