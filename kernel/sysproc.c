#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#define SCALE 1000

//proc.c
extern struct proc proc[];

uint64 mem_access_time = 0; // (bulwark)
uint64 mem_access_count = 0; // (bulwark)

// kalloc.c
extern uint64 mem_alloc_time; // (bulwark)
extern uint64 mem_free_time; // (bulwark)
extern uint64 mem_alloc_count; // (bulwark)
extern uint64 mem_free_count; // (bulwark)

// sysfile.c
extern uint64 fs_read_time; // (bulwark)
extern uint64 fs_write_time; // (bulwark)
extern uint64 fs_unlink_time; // (bulwark)
extern int fs_read_count; // (bulwark)
extern int fs_write_count; // (bulwark)
extern int fs_unlink_count; // (bulwark)

// trap.c
extern uint ticks; //(bulwark)

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;

  uint64 start_growproc = ticks; // (bulwark)
  
  if(growproc(n) < 0)
    return -1;

  //Calculating growproc time and count (bulwark)
  uint64 end_growproc = ticks;
  mem_access_time += end_growproc - start_growproc;
  mem_access_count++;

  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

//Calculates the fairness multiplied by SCALE (bulwark)
uint64
sys_fairness(void)
{
  struct proc *p;

  uint64 num_proc = 0;
  uint64 total_processing_time = 0;
  uint64 total_processing_time_squared = 0;

  for (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if (p->state == ZOMBIE) {
      num_proc++;
      total_processing_time += p->elapsed_time;
      total_processing_time_squared += p->elapsed_time * p->elapsed_time;
    }
    release(&p->lock);
  }

  if(num_proc == 0 || total_processing_time_squared == 0) return 0;

  // SCALE * sum(x)^2/ (N * sum(x^2))
  uint64 fairness_scaled = SCALE * total_processing_time * total_processing_time;
  fairness_scaled = fairness_scaled / num_proc;
  fairness_scaled = fairness_scaled / total_processing_time_squared;

  return fairness_scaled;
}

//Calculates the throughput multiplied by SCALE (bulwark)
uint64
sys_throughput(void)
{
  struct proc *p;

  uint64 num_proc = 0;
  uint64 total_processing_time = 0;

  for (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if (p->state == ZOMBIE) {
      num_proc++;
      total_processing_time += p->elapsed_time;
    }
    release(&p->lock);
  }

  if(total_processing_time == 0) return 0;

  // Since 10^4 ticks ~ 1 second, we can perform the following calculations:
  // num_proc / seconds
  // num_proc / (total_processing_time / 10^4)
  // 10^4 * num_proc / (total_processing_time)
  uint64 throughput_calc =  10000 * num_proc;
  throughput_calc = throughput_calc / total_processing_time;

  return throughput_calc;
}

// Função para calcular a eficiência do sistema de arquivos
uint64 
sys_fsefficiency(void) {

    if(fs_read_count == 0 || fs_write_count == 0 || fs_unlink_count == 0) return 0;
    
    // Cálculo dos tempos médios
    uint64 T_read = SCALE * fs_read_time / fs_read_count;
    uint64 T_write = SCALE * fs_write_time / fs_write_count;
    uint64 T_delete = SCALE * fs_unlink_time / fs_unlink_count;

    // Soma total dos tempos
    uint64 total_time = T_read + T_write + T_delete;

    // Verificação para evitar divisão por zero
    if (total_time == 0) {
        return 0; // Eficiência mínima (sem operações suficientes)
    }

    // SCALE * SCALE * 1/(SCALE * T) = SCALE / T
    return (SCALE * SCALE / total_time); // Eficiência do sistema de arquivos
}

// Função para calcular o Overhead de Gerenciamento de Memória
uint64 
sys_memoverhead() {

    if(mem_alloc_count == 0 || mem_access_count == 0 || mem_free_count == 0) return 0;

    // Cálculo dos tempos médios
    uint64 T_alloc = SCALE * mem_alloc_time / mem_alloc_count;
    uint64 T_access = SCALE * mem_access_time / mem_access_count;
    uint64 T_free = SCALE * mem_free_time / mem_free_count;

    // Soma total dos tempos
    uint64 total_time = T_alloc + T_access + T_free;

    // Verificação para evitar divisão por zero
    if (total_time == 0) {
        return 0; // Nenhuma operação suficiente para calcular
    }

    // SCALE * SCALE * 1/(SCALE * T) = SCALE / T
    return (SCALE * SCALE / total_time); // Calcula o Overhead
}