#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>

#define DRIVER_AUTHOR "Son Giang <giangsondh@gmail.com>"
#define DRIVER_DESC "A sample character device driver"
#define DRIVER_VERSION "0.4"

struct _vchar_drv {
	dev_t dev_num; // charDev = Major, minor
	struct class *dev_class; // create class device in /sys/class 
	struct device *dev; // create info device
} vchar_drv;

/*ham khoi tao driver */
static int __init vchar_driver_init(void)
{
    int ret = 0;
	/*cap phat device number */
    vchar_drv.dev_num = 0;
  ret = alloc_chrdev_region(&vchar_drv.dev_num, 0, 1, "vchar_device");

    if (ret < 0) {
        printk("failed to register device number dynamically\n");
        goto failed_register_devnum;
    }
 printk("allocated device number (%d,%d)\n", MAJOR(vchar_drv.dev_num), MINOR(vchar_drv.dev_num));
	/*tao device file */
vchar_drv.dev_class = class_create(THIS_MODULE, "class_vchar_dev");
if(vchar_drv.dev_class == NULL){
	printk("failed to create a device class\n");
	goto failed_create_class;
}

vchar_drv.dev = device_create(vchar_drv.dev_class, NULL, vchar_drv.dev_num, NULL, "vchar_dev");
  if(IS_ERR(vchar_drv.dev)){
          printk("failed to create a device\n");    
          goto failed_create_device;
  } 	
    printk("Initialize vchar driver successfully\n");
    return 0;

failed_create_device:
      class_destroy(vchar_drv.dev_class);
failed_create_class:
      unregister_chrdev_region(vchar_drv.dev_num, 1);
failed_register_devnum:
    return ret;
}
	/*xoa bo device file */

	/*giai phong device number*/
void __exit vchar_driver_exit(void)
{
	device_destroy(vchar_drv.dev_class, vchar_drv.dev_num);
	class_destroy(vchar_drv.dev_class);

    unregister_chrdev_region(vchar_drv.dev_num, 1);
    printk("Exit vchar driver\n");
}

module_init(vchar_driver_init);
module_exit(vchar_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);

