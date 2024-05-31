/**
 * @file report.h
 * @author Marcos Dominguez (mrds0690@gmail.com)
 * @brief Manage reports
 * @version 0.1
 * @date 2024-05-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef REPORT_H
#define REPORT_H

#include <stdint.h>

#define ENABLE_MEASUREMENT_MEASUREMENT 1
#define DISABLE_MEASUREMENT_MEASUREMENT 0

void REPORT_MEASUREMENTReportEnable(uint8_t enable);

uint8_t REPORT_MEASUREMENTReportCheck(void);


#endif /* REPORT_H */