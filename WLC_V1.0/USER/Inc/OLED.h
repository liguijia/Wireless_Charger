/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       oled.c/h
  * @brief      0.96 inch oled use SSD1306 driver. the file includes oled initialization function,
  *             and some OLED setting function, GRAM operate function, oled show num ,char and string function,
  *             show RoboMaster LOGO function.
  *             0.96OLEDʹ��SSD1306�����������ļ�������ʼ�������Լ�����OLED���ú����� GRAM����������oled��ʾ���֣��ַ����ַ�������
  *             �Լ���ʾRoboMaster LOGO����
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. done
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */

#ifndef OLED_H
#define OLED_H
#include <stdint.h>


// the I2C address of oled
#define OLED_I2C_ADDRESS    0x78

//the resolution of oled   128*64
#define MAX_COLUMN      128
#define MAX_ROW         64

#define X_WIDTH         MAX_COLUMN
#define Y_WIDTH         MAX_ROW

#define OLED_CMD        0x00
#define OLED_DATA       0x01

#define CHAR_SIZE_WIDTH 6
#define CHAR_SIZE_HIGHT 12


typedef enum
{
    PEN_CLEAR = 0x00,
    PEN_WRITE = 0x01,
    PEN_INVERSION= 0x02,
}pen_typedef;



/**
  * @brief          initialize the oled device
  * @param[in]      none
  * @retval         none
  */
/**
  * @brief          ��ʼ��OLEDģ�飬
  * @param[in]      none
  * @retval         none
  */
extern void OLED_init(void);


/**
  * @brief          turn on OLED display
  * @param[in]      none
  * @retval         none
  */
/**
  * @brief          ��OLED��ʾ
  * @param[in]      none
  * @retval         none
  */
extern void OLED_display_on(void);


/**
  * @brief          turn off OLED display
  * @param[in]      none
  * @retval         none
  */
/**
  * @brief          �ر�OLED��ʾ
  * @param[in]      none
  * @retval         none
  */
extern void OLED_display_off(void);


/**
  * @brief          operate the graphic ram(size: 128*8 char)
  * @param[in]      pen: the type of operate.
                    PEN_CLEAR: set ram to 0x00
                    PEN_WRITE: set ram to 0xff
                    PEN_INVERSION: bit inversion 
  * @retval         none
  */
/**
  * @brief          ����GRAM�ڴ�(128*8char����)
  * @param[in]      pen: ��������.
                    PEN_CLEAR: ����Ϊ0x00
                    PEN_WRITE: ����Ϊ0xff
                    PEN_INVERSION: ��λȡ��
  * @retval         none
  */
extern void OLED_operate_gram(pen_typedef pen);


/**
  * @brief          cursor set to (x,y) point
  * @param[in]      x:X-axis, from 0 to 127
  * @param[in]      y:Y-axis, from 0 to 7
  * @retval         none
  */
/**
  * @brief          ���ù�����(x,y)
  * @param[in]      x:x��, �� 0 �� 127
  * @param[in]      y:y��, �� 0 �� 7
  * @retval         none
  */
extern void OLED_set_pos(uint8_t x, uint8_t y);


/**
  * @brief          draw one bit of graphic raw, operate one point of screan(128*64)
  * @param[in]      x: x-axis, [0, X_WIDTH-1]
  * @param[in]      y: y-axis, [0, Y_WIDTH-1]
  * @param[in]      pen: type of operation,
                        PEN_CLEAR: set (x,y) to 0
                        PEN_WRITE: set (x,y) to 1
                        PEN_INVERSION: (x,y) value inversion 
  * @retval         none
  */
/**
  * @brief          ����GRAM�е�һ��λ���൱�ڲ�����Ļ��һ����
  * @param[in]      x:x��,  [0,X_WIDTH-1]
  * @param[in]      y:y��,  [0,Y_WIDTH-1]
  * @param[in]      pen: ��������,
                        PEN_CLEAR: ���� (x,y) ��Ϊ 0
                        PEN_WRITE: ���� (x,y) ��Ϊ 1
                        PEN_INVERSION: (x,y) ֵ��ת
  * @retval         none
  */
extern void OLED_draw_point(int8_t x, int8_t y, pen_typedef pen);


/**
  * @brief          draw a line from (x1, y1) to (x2, y2)
  * @param[in]      x1: the start point of line
  * @param[in]      y1: the start point of line
  * @param[in]      x2: the end point of line
  * @param[in]      y2: the end point of line
  * @param[in]      pen: type of operation,PEN_CLEAR,PEN_WRITE,PEN_INVERSION.
  * @retval         none
  */
/**
  * @brief          ��һ��ֱ�ߣ���(x1,y1)��(x2,y2)
  * @param[in]      x1: ���
  * @param[in]      y1: ���
  * @param[in]      x2: �յ�
  * @param[in]      y2: �յ�
  * @param[in]      pen: ��������,PEN_CLEAR,PEN_WRITE,PEN_INVERSION.
  * @retval         none
  */
extern void OLED_draw_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, pen_typedef pen);


/**
  * @brief          show a character
  * @param[in]      row: start row of character
  * @param[in]      col: start column of character
  * @param[in]      chr: the character ready to show
  * @retval         none
  */
/**
  * @brief          ��ʾһ���ַ�
  * @param[in]      row: �ַ��Ŀ�ʼ��
  * @param[in]      col: �ַ��Ŀ�ʼ��
  * @param[in]      chr: �ַ�
  * @retval         none
  */
extern void OLED_show_char(uint8_t row, uint8_t col, uint8_t chr);

/**
  * @brief          show a character string
  * @param[in]      row: row of character string begin
  * @param[in]      col: column of character string begin
  * @param[in]      chr: the pointer to character string
  * @retval         none
  */
/**
  * @brief          ��ʾһ���ַ���
  * @param[in]      row: �ַ����Ŀ�ʼ��
  * @param[in]      col: �ַ����Ŀ�ʼ��
  * @param[in]      chr: �ַ���
  * @retval         none
  */
extern void OLED_show_string(uint8_t row, uint8_t col, uint8_t *chr);

/**
  * @brief          formatted output in oled 128*64
  * @param[in]      row: row of character string begin, 0 <= row <= 4;
  * @param[in]      col: column of character string begin, 0 <= col <= 20;
  * @param          *fmt: the pointer to format character string
  * @note           if the character length is more than one row at a time, the extra characters will be truncated
  * @retval         none
  */
/**
  * @brief          ��ʽ���
  * @param[in]      row: ��ʼ�У�0 <= row <= 4;
  * @param[in]      col: ��ʼ�У� 0 <= col <= 20;
  * @param[in]      *fmt:��ʽ������ַ���
  * @note           ����ַ������ȴ���һ�У�������ַ��ỻ��
  * @retval         none
  */
extern void OLED_printf(uint8_t row, uint8_t col, const char *fmt,...);

/**
  * @brief          send the data of gram to oled sreen
  * @param[in]      none
  * @retval         none
  */
/**
  * @brief          �������ݵ�OLED��GRAM
  * @param[in]      none
  * @retval         none
  */
extern void OLED_refresh_gram(void);


/**
  * @brief          show the logo of RoboMaster
  * @param[in]      none
  * @retval         none
  */
/**
  * @brief          ��ʾRM��LOGO
  * @param[in]      none
  * @retval         none
  */
extern void OLED_LOGO(void);

extern void OLED_SetRotation(uint8_t rotation);

#endif
