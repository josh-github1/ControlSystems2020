#include "L3_Application/task.hpp"
#include "utility/log.hpp"
#include "utility/rtos.hpp"

class PrinterTask final : public sjsu::rtos::Task<512>
{
 public:
  PrinterTask(const char * task_name,
              const char * message,
              sjsu::rtos::TaskSchedulerInterface & task_scheduler)
      : Task(task_name, sjsu::rtos::Priority::kMedium, task_scheduler),
        message_(message),
        run_count_(0)
  {
    print_mutex = NULL;
  }

  bool Setup() override
  {
    if (print_mutex == NULL)
    {
      // print_mutex = xSemaphoreCreateMutexStatic(&print_mutex_stack_buffer);
      print_mutex = xSemaphoreCreateMutex();
    }
    sjsu::LogInfo("Completed Setup() for: %s", GetName());
    return true;
  }

  bool PreRun() override
  {
    xSemaphoreTake(print_mutex, portMAX_DELAY);
    sjsu::LogInfo("Completed PreRun() for: %s", GetName());
    xSemaphoreGive(print_mutex);
    return true;
  }

  bool Run() override
  {
    run_count_ += 1;
    xSemaphoreTake(print_mutex, portMAX_DELAY);
    sjsu::LogInfo("%s: %lu", message_, run_count_);
    xSemaphoreGive(print_mutex);
    if (run_count_ == 10)
    {
      sjsu::LogInfo("Deleting: %s", GetName());
      Delete();
    }
    return true;
  }

 private:
  // inline static StaticSemaphore_t print_mutex_stack_buffer;
  inline static SemaphoreHandle_t print_mutex;

  const char * message_;
  uint32_t run_count_;
};

sjsu::rtos::TaskScheduler scheduler;
PrinterTask printer_one("Printer A", "I am a printer, I am faster", scheduler);
PrinterTask printer_two("Printer B", "I am also a printer", scheduler);

int main()
{
  sjsu::LogInfo("Starting TaskScheduler example...");
  // setting Printer A to print 2 times faster than Printer B
  printer_one.SetDelayTime(500);
  printer_two.SetDelayTime(1000);

  sjsu::LogInfo("Attempting to search for Printer A in the scheduler...");
  sjsu::rtos::TaskInterface * task = scheduler.GetTask("Printer A");

  if (task != nullptr)
  {
    sjsu::LogInfo("Found task: %s", task->GetName());
  }
  else
  {
    sjsu::LogError("Could not find task \"Printer A\", Halt System.");
    sjsu::Halt();
  }

  sjsu::LogInfo("Starting scheduler");
  scheduler.Start();
  sjsu::LogInfo("This point should not be reached!");
  return 0;
}
