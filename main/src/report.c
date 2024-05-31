/**
 * @file report.c
 * @author Marcos Dominguez (mrds0690@gmail.com)
 * @brief Manage reports
 * @version 0.1
 * @date 2024-05-16
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "report.h"

static uint8_t measurement_rep_enable = 0;

void REPORT_MEASUREMENTReportEnable(uint8_t enable) {
    measurement_rep_enable=enable;
}


uint8_t REPORT_MEASUREMENTReportCheck(void) {
    return measurement_rep_enable;
}