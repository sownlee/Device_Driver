/*
 * mo ta : char driver cho thiet bi gia lap vchar_device.
 * vchar_device la mot thiet bi nam tren RAM.
 */
#include <linux/module.h> /* thu vien nay dinh nghia cac macro nhu module_init va module_exit */
#include <linux/fs.h>
#define DRIVER_AUTHOR "Son Giang <giangsondh@gmail.com>"
#define DRIVER_DESC "A sample character device driver"
#define DRIVER_VERSION "0.3"
/****************************** device specific - START *****************************/
struct __vchar_drv {
    dev_t dev_num;
} vchar_drv;
/* ham khoi tao thiet bi */
/* ham giai phong thiet bi */
/* ham doc tu cac thanh ghi du lieu cua thiet bi */
/* ham ghi vao cac thanh ghi du lieu cua thiet bi */
/* ham doc tu cac thanh ghi trang thai cua thiet bi */
/* ham ghi vao cac thanh ghi dieu khien cua thiet bi */
/* ham xu ly tin hieu ngat gui tu thiet bi */
/******************************* device specific - END *****************************/
/******************************** OS specific - START *******************************/
/* cac ham entry points */
/* ham khoi tao driver */
static int __init vchar_driver_init(void)
{
    int ret = 0;
// struct vchar_drv *drv = &vchar_drv;
    /* cap phat device number */
    //	struct vchar_drv *drv = &vchar_drv;

	// drv->dev_num = MKDEV(235, 0);	
    vchar_drv.dev_num = MKDEV(235, 0);
//ret = register_chrdev_region(drv->dev_num, 1, "vchar_device")    ;

   ret = register_chrdev_region(vchar_drv.dev_num, 1, "vchar_device");
	 if (ret < 0) {
        printk("failed to register number statically\n");
        goto failed_register_devnum;
    }
    /* tao device file */
    /* cap phat bo nho cho cac cau truc du lieu cua driver va khoi tao */
    /* khoi tao thiet bi vat ly */
    /* dang ky cac entry point voi kernel */
    /* dang ky ham xu ly ngat */
    printk("Initialize vchar driver successfully\n");
    return 0;

failed_register_devnum:
    return ret;
}
/* ham ket thuc driver */
static void __exit vchar_driver_exit(void)
{
    //	struct vchar_drv *drv = &vchar_drv;

    /* huy dang ky xu ly ngat */
    /* huy dang ky entry point voi kernel */
    /* giai phong thiet bi vat ly */
    /* giai phong bo nho da cap phat cau truc du lieu cua driver */
    /* xoa bo device file */
    /* giai phong device number */
//	unregister_chrdev_region(drv->dev_num, 1);	
    unregister_chrdev_region(vchar_drv.dev_num, 1);
    printk("Exit vchar driver\n");
}
/********************************* OS specific - END ********************************/
module_init(vchar_driver_init);
module_exit(vchar_driver_exit);
MODULE_LICENSE("GPL"); /* giay phep su dung cua module */
MODULE_AUTHOR(DRIVER_AUTHOR); /* tac gia cua module */
MODULE_DESCRIPTION(DRIVER_DESC); /* mo ta chuc nang cua module */
//MODULE_VERSION(DRIVER_VERSION); /* mo ta phien ban cuar module */
//MODULE_SUPPORTED_DEVICE("testdevice"); /* kieu device ma module ho tro */

