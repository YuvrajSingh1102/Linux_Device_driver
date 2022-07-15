#include<linux/module.h>
#include<linux/init.h>
#include<linux/moduleparam.h>

int myint[3] = {0, 0, 0};

module_param_array(myint, int, NULL, S_IRUSR | S_IWUSR);

void display(void) {
	pr_info("The myint value is %d\n", myint[0]);
	pr_info("The myint value is %d\n", myint[1]);
	pr_info("The myint value is %d\n", myint[2]);
}
static int __init helloworld_init(void) 
{
	pr_info("Hello World\n");
	display();
	return 0;
}

static void __exit helloworld_cleanup(void) 
{
	pr_info("Good bye World\n");
}

//These are module registration.

module_init(helloworld_init);
module_exit(helloworld_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ME");
MODULE_DESCRIPTION("A simple hello world kernel module");
MODULE_INFO(board,"Beaglebone black REV A5");
