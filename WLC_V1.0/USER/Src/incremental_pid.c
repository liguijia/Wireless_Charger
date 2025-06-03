#include "incremental_pid.h"
#include <stdio.h>
#include <string.h> // For memset

/* 初始化增量式PID控制器 */
void incremental_pid_init(incremental_pid_t *pid, float kp, float ki, float kd, float min_output, float max_output)
{
    memset(pid, 0, sizeof(incremental_pid_t));
    pid->Kp             = kp;
    pid->Ki             = ki;
    pid->Kd             = kd;
    pid->outputMinLimit = min_output;
    pid->outputMaxLimit = max_output;
}

/* 增量式PID算法计算
@param pid  要计算的pid结构体指针
返回给执行器的输出增量值
*/
float incremental_pid_compute(incremental_pid_t *pid, float newActualValue, float setValue)
{
    // 更新实际值
    pid->setValue    = setValue;
    pid->actualValue = newActualValue;

    float increment;
    pid->error = pid->setValue - pid->actualValue;
    pid->P     = pid->Kp * (pid->error - pid->errorPre);                        // 比例项
    pid->I     = pid->Ki * pid->error;                                          // 积分项
    pid->D     = pid->Kd * (pid->error - 2 * pid->errorPre + pid->errorPrePre); // 微分项
    increment  = pid->P + pid->I + pid->D;                                      // PID计算得到的输出增量

    // 计算新输出并应用限制
    pid->output += increment;
    if (pid->output > pid->outputMaxLimit) {
        pid->output = pid->outputMaxLimit;
    } else if (pid->output < pid->outputMinLimit) {
        pid->output = pid->outputMinLimit;
    }

    pid->errorPrePre = pid->errorPre;
    pid->errorPre    = pid->error;

    return pid->output;
}

/* 重置增量式PID控制器 */
void incremental_pid_reset(incremental_pid_t *pid)
{
    // 保存不需要重置的参数
    float kp         = pid->Kp;
    float ki         = pid->Ki;
    float kd         = pid->Kd;
    float setValue   = pid->setValue;
    float min_output = pid->outputMinLimit;
    float max_output = pid->outputMaxLimit;

    // 使用memset重置所有其他成员变量
    memset(pid, 0, sizeof(incremental_pid_t));

    // 恢复不需要重置的参数
    pid->Kp             = kp;
    pid->Ki             = ki;
    pid->Kd             = kd;
    pid->setValue       = setValue;
    pid->outputMinLimit = min_output;
    pid->outputMaxLimit = max_output;
}