#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charlie Sands");
MODULE_DESCRIPTION("Linux driver for Sckzor's 2.4ish GHz Radar");

#define USB_VID 0x1209
#define USB_PID 0x5955

static struct usb_device_id radar_usb_table[] = {
	{ USB_DEVICE(USB_VID, USB_PID) },
	{},
};

MODULE_DEVICE_TABLE(usb, radar_usb_table);

static int radar_usb_probe(struct usb_interface *intf, const struct usb_device_id *id) {
	printk("radar_usb_devdrv probe\n");
	return 0;
}

static void radar_usb_disconnect(struct usb_interface *intf) {
	printk("radar_usb_devdrv disconnect\n");
}

static struct usb_driver radar_usb_driver = {
	.name = "radar_usb_devdrv",
	.id_table = radar_usb_table,
	.probe = radar_usb_probe,
	.disconnect = radar_usb_disconnect,
};

static int __init radar_init(void) {
	int result;
	printk("radar_usb_devdrv load\n");

	result = usb_register(&radar_usb_driver);
	if(result) {
		printk("radar_usb_devdrv usb_register error");
		return -1;
	}

	return 0;
}

static void __exit radar_exit(void) {
	printk("radar_usb_devdrv exit\n");
	
	usb_deregister(&radar_usb_driver);
}

module_init(radar_init);
module_exit(radar_exit);
