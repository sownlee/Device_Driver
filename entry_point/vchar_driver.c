#include <linux/module.h>
#include <linux/fs.h> // ham cap phat/giai phong device number
#include <linux/device.h> // create ddevice file
#include <linux/uaccess.h> 
#include <linux/slab.h> // kmalloc and kfree
#include <linux/cdev.h> // cdev

#define DRIVER_AUTHOR "Son Giang <giangsondh@gmail.com>"
#define DRIVER_DESC "A sample character device driver"
#define DRIVER_VERSION "0.6"

// Định nghĩa cấu trúc vchar_dev_t
typedef struct {
    unsigned int *control_regs;
    unsigned int *status_regs;
    unsigned int *data_regs;
} vchar_dev_t;

#define NUM_DEV_REGS 32
#define REG_SIZE sizeof(unsigned int)
#define NUM_CTRL_REGS 8
#define NUM_STS_REGS 8
#define CONTROL_ACCESS_REG 0
#define DEVICE_STATUS_REG 1

typedef struct _vchar_drv {
    dev_t dev_num; // charDev = Major, minor
    struct class *dev_class; // create class device in /sys/class
    struct device *dev; // create info device
    vchar_dev_t *vchar_hw; // Cấu trúc cho thiết bị vchar
    struct cdev *vcdev;
    unsigned int open_cnt;
} vchar_drv;

vchar_drv vchar_driver; // Declare a global instance of vchar_drv

/* Hàm khởi tạo thiết bị */
int vchar_hw_init(vchar_dev_t *hw) {
    char *buf;

// Use kzalloc from linux/slab.h
    buf = kzalloc(NUM_DEV_REGS * REG_SIZE, GFP_KERNEL);
    if (!buf) {
        return -ENOMEM;
    }

    hw->control_regs = (unsigned int *)buf;
    hw->status_regs = (unsigned int *)(hw->control_regs + NUM_CTRL_REGS);
    hw->data_regs = (unsigned int *)(hw->status_regs + NUM_STS_REGS);

    // Khởi tạo giá trị ban đầu cho các thanh ghi
    hw->control_regs[CONTROL_ACCESS_REG] = 0x03;
    hw->status_regs[DEVICE_STATUS_REG] = 0x03;

    return 0;
}

/* Hàm giải phóng thiết bị */
void vchar_hw_exit(vchar_dev_t *hw) {
    kfree(hw->control_regs);
}

/* Hàm đọc từ các thanh ghi dữ liệu của thiết bị */
/* Hàm ghi vào các thanh ghi dữ liệu của thiết bị */
/* Hàm đọc từ các thanh ghi trạng thái của thiết bị */
/* Hàm ghi vào các thanh ghi điều khiển của thiết bị */
/* Hàm xử lý tín hiệu ngắt gửi từ thiết bị */

/********************************* OS specific - START *************************/

/* Các hàm entry points */
static int vchar_driver_open(struct inode *inode, struct file *filp)
{
    vchar_driver.open_cnt++;
    printk("handle opened event (%d)\n", vchar_driver.open_cnt);
    return 0;
}

static int vchar_driver_release(struct inode *inode, struct file *filp)
{
    printk("handle closed event\n");
    return 0;
}

static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = vchar_driver_open,
    .release = vchar_driver_release,
};

/* Hàm khởi tạo driver */
static int __init vchar_driver_init(void) {
    int ret = 0;

    // Cấp phát device number
    vchar_driver.dev_num = 0;
    ret = alloc_chrdev_region(&vchar_driver.dev_num, 0, 1, "vchar_device");

    if (ret < 0) {
        printk("Failed to register device number dynamically\n");
        goto failed_register_devnum;
    }

    printk("Allocated device number (%d,%d)\n", MAJOR(vchar_driver.dev_num), MINOR(vchar_driver.dev_num));

    // Tạo device file
    vchar_driver.dev_class = class_create(THIS_MODULE, "class_vchar_dev");
    if (vchar_driver.dev_class == NULL) {
        printk("Failed to create a device class\n");
        goto failed_create_class;
    }

    vchar_driver.dev = device_create(vchar_driver.dev_class, NULL, vchar_driver.dev_num, NULL, "vchar_dev");
    if (IS_ERR(vchar_driver.dev)) {
        printk("Failed to create a device\n");
        goto failed_create_device;
    }

    // Cấp phát bộ nhớ cho các cấu trúc dữ liệu của driver và khởi tạo
    vchar_driver.vchar_hw = kzalloc(sizeof(vchar_dev_t), GFP_KERNEL);
    if (!vchar_driver.vchar_hw) {
        printk("Failed to allocate data structure of the driver\n");
        ret = -ENOMEM;
        goto failed_allocate_structure;
    }

    // Khởi tạo thiết bị vật lý
    ret = vchar_hw_init(vchar_driver.vchar_hw);
    if (ret < 0) {
        printk("Failed to initialize a virtual character device\n");
        goto failed_init_hw;
    }

    // Đăng ký các entry point với kernel
    vchar_driver.vcdev = cdev_alloc();
    if(vchar_driver.vcdev == NULL){
        printk("failed to allocate cdev structure\n");
        goto failed_allocate_cdev;
    }
    cdev_init(vchar_driver.vcdev,&fops);
    ret = cdev_add(vchar_driver.vcdev, vchar_driver.dev_num,1);
    if(ret<0) {
        printk("failed to add a char device to the system\n");
        goto failed_allocate_cdev;
    }

    printk("Initialize vchar driver successfully\n");
    return 0;

failed_allocate_cdev:
    vchar_hw_exit(vchar_driver.vchar_hw);
failed_init_hw:
    kfree(vchar_driver.vchar_hw);
failed_allocate_structure:
    device_destroy(vchar_driver.dev_class, vchar_driver.dev_num);
failed_create_device:
    class_destroy(vchar_driver.dev_class);
failed_create_class:
    unregister_chrdev_region(vchar_driver.dev_num, 1);
failed_register_devnum:
    return ret;
}

/* Hàm kết thúc driver */
void __exit vchar_driver_exit(void) {
    class_destroy(vchar_driver.dev_class);

    // Hủy đăng ký xử lý ngắt

    // Hủy đăng ký entry point với kernel
    cdev_del(vchar_driver.vcdev);

    // Giải phóng thiết bị vật lý
    vchar_hw_exit(vchar_driver.vchar_hw);

    // Giải phóng bộ nhớ đã cấp phát cho cấu trúc dữ liệu của driver
    kfree(vchar_driver.vchar_hw);

    // Xóa bỏ device file
    device_destroy(vchar_driver.dev_class, vchar_driver.dev_num);

    // Giải phóng device number
    unregister_chrdev_region(vchar_driver.dev_num, 1);

    printk("Exit vchar driver\n");
}

/********************************* OS specific - END ***************************/

module_init(vchar_driver_init);
module_exit(vchar_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
