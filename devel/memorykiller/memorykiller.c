#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/mm.h>

#define RSS_THRESHOLD 7000

asmlinkage long(*sys_tkill)(int pid , int sig);

struct sorted_tasks {
	struct task_struct *task;
	struct my_tasks *prev;
	struct my_tasks *next;
}

static insert_node(struct task_struct *task, struct sorted_tasks **head) {
	struct sorted_tasks *p;
	struct sorted_tasks *new_node;

	new_node = kmalloc(sizeof(struct sorted_tasks), GFP_KERNEL);

	// list is empty
	if (*head == NULL) {
		printk("List is empty, insert to head\n");
		new_node->task = task;
		new_node->next = NULL;
		new_node->prev = NULL;
		*head = new_node;
	}
	else {
		p = *head;
		while(1) {
			if (get_mm_rss(task->mm) < get_mm_rss(p->task->mm)) {
				// end of list
				if (p->next == NULL) {
					new_node->task = task;
					new_node->next = NULL;
					new_node->prev = p;
					p->next = new_node;
					break;
				}

				p = p->next;
			}
			else {
				new_node->task = task;
				new_node->next = p;
				new_node->prev = p->prev;
				if (p->prev != NULL) {
					struct sorted_tasks *prev = p->prev;
					prev->next = new_node;
				}
				p->prev = new_node;
				break;
			}
		}
	}
}

static int traverse_nodes(struct sorted_tasks **head) {
	struct sorted_tasks *p;
	p = *head;
	printk("%5s %8s\n", "PID", "RSS");
	while (p != NULL) {
		printk("%5d %8lu\n", p->task->pid, get_mm_rss(p->task->mm));
		p = p->next;
	}
	return 0;
}

static int kill_the_highest(struct sorted_tasks **head) {
	unsigned long *sys_call_table = (unsigned long *)0xc1269280;
	sys_tkill = sys_call_table[__NR_tkill];
	struct sorted_tasks *p;

	p = *head;
	if (get_mm_rss(p->task->mm) > RSS_THRESHOLD) {
		printk("pid %d use too much memory, killed!\n", p->task->pid);
		sys_tkill(p->task->pid , SIGIOT);
	}

	return 0;
}

static int free_nodes(struct sorted_tasks **head) {
	struct sorted_tasks *p;
	struct sorted_tasks *victom;

	p = *head;
	while (p != NULL) {
		victom = p;
		p = p->next;
		kfree(victom);
	}
}

static int memorykiller(void *data) {
	struct task_struct *p = get_current();
	struct task_struct *entry = NULL;
	struct mm_struct *mm = NULL;
	struct sorted_tasks **my_tasks = NULL;

	daemonize("memorykiller");

	while(1) {
		list_for_each_entry(entry, &(p->tasks), tasks) {
			if (entry != NULL) {
				mm = entry->mm;
				if(mm) {
					insert_node(entry, &my_tasks);
				}
			}
		}

		traverse_nodes(&my_tasks);
		kill_the_highest(&my_tasks);
		free_nodes(&my_tasks);
		my_tasks = NULL;

		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(10 * HZ);
	}
	return 0;
}

static int memorykiller_init(void) {
	kernel_thread(memorykiller, NULL, CLONE_KERNEL);
	return 0;
}

static void memorykiller_exit(void) {}

module_init(memorykiller_init);
module_exit(memorykiller_exit);
