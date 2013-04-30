#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/hardirq.h>
#include <linux/highmem.h>
#include <linux/vmstat.h>
#include <linux/memcontrol.h>
#include <linux/mm_inline.h>
#include <linux/swap.h>
#include <linux/gfp.h>

static int pfinfo(void *data) {
	struct task_struct *p = get_current();
	struct task_struct *entry = NULL;
	struct mm_struct *mm = NULL;

	daemonize("pfinfo");

	while(1) {
		list_for_each_entry(entry, &(p->tasks), tasks) {
			if (entry != NULL) {
				int counter = 0;
				if (strcmp(entry->comm, "memoryconsumer") == 0) {
					printk("PID: %d\n", entry->pid);
					mm = entry->mm;
					if(mm) {
						struct vm_area_struct *vma = mm->mmap;
						while (vma != NULL) {
							unsigned long address;
							for (address = vma->vm_start; address < vma->vm_end; address += PAGE_SIZE) {
								pgd_t *pgd = NULL;
								pud_t *pud = NULL;
								pmd_t *pmd = NULL;
								pte_t *pte = NULL;
								spinlock_t *ptl;
								int flag = 0;
								unsigned long num = 0;
								struct page *page = NULL;
								struct zone *zone = NULL;
								int lru = 0;

								pgd = pgd_offset(mm, address);
								if (!pgd_present(*pgd)) {
									printk("Cannot find pgd\n");
									flag = 1;
								}

								pud = pud_offset(pgd, address);
								if (!pud_present(*pud)) {
									printk("Cannot find pud\n");
									flag = 1;
								}

								pmd = pmd_offset(pud, address);
								if (!pmd_present(*pmd)) {
									printk("Cannot find pmd\n");
									flag = 1;
								}

								pte = pte_offset_map(pmd, address);
								if (!pte_present(*pte)) {
									printk("Cannot find pte\n");
									flag = 1;
								}

								if (flag == 0) {
									counter++;
									printk("Counter: %d\n", counter);
								}

								pte_unmap(pte);
								//ptl = pte_lockptr(mm , pmd);
								//spin_lock(ptl);
								//num = pte_pfn(*pte);
								//page = pfn_to_page(num);
								//pte_unmap_unlock(pte, ptl);

								/*
								if (page != NULL) {
									lru = page_lru(page);
									zone = page_zone(page);
									list_add(&page->lru, &zone->lru[lru].list);
								}
								*/
							}
							vma = vma->vm_next;
						}
					}
				}
			}
		}

		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(10 * HZ);
	}
	return 0;
}

static int pfinfo_init(void) {
	kernel_thread(pfinfo, NULL, CLONE_KERNEL);
	return 0;
}

static void pfinfo_exit(void) {}

module_init(pfinfo_init);
module_exit(pfinfo_exit);
