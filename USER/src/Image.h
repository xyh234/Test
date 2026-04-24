/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Copyright (c) 2008 by MKS Controls
 *
 *
 *  File name: Image.h
 *  Module:    Application module
 *  Language:  ANSI C
 *  $Revision: 1 $
 *
 *  GENERAL DESCRIPTION
 *  This is a software object for functions to generate an output image
 *  for a fixed-segment display.  The module does not control hardware outputs
 *  to drive a display. The module only constructs an image in memory to be
 *  displayed and is optionally communicated through a COM port. It has an
 *  optional capability to handle two images:  the image being displayed and a
 *  working image. The module accepts high-level requests such as characters to
 *  be displayed and determines which segments should be turned on.
 *
 *  CONSTRAINTS
 *  The index of blank character must be same in all character sets.
 *  The index of all segments on character must be same in all character sets.
 *  Message ID numbers must conform to Standard Messages specification.
 *  Module Transport 0809.
 *  Module Ram 0301.
 *  Limitations - The module will not process new messages until
 *  the processing of an existing message is finished.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "newtype.h"
 
#ifndef IMAGE_H
#define IMAGE_H


/*------------------*
 |  Module options
 *------------------*/


/*--------------------*
|  Public functions
*--------------------*/


/*---------------------------------------------------------------------------*
 |  ImageAll
 |
 |  Turn on the entire display. See Image.h for details
 *---------------------------------------------------------------------------*/
void ImageAll(uint8_t num);
void ImageTime(uint8_t minute, uint8_t second);
void ImageTime_S(uint16_t second);
void ImageTime_1(uint8_t hour, uint8_t minute, uint8_t second);
void ImageTime2(uint16_t nm);
void ImageTime1(uint8_t minute, uint8_t second);
//void ImageTime_Dist(uint8_t minute, uint8_t second);
void ImageWatts(uint16_t watts);
void ImageSPM(uint16_t spm);
void ImageCal(uint16_t cal);
void ImageDist(uint32_t dist);
//void ImageDist2(uint32_t dist);
//void ImageMSG(uint32_t msg);
//void ImageListTime(uint16_t listTime);//(uint8_t min, uint8_t sec);
void ImageList(uint16_t list);
void ImageINTERVALS(uint16_t intervals);
//void ImageINTERVALS1(uint8_t intervals1);
//void ImageINTERVALS2(uint8_t intervals2);
void ImageHeart(uint16_t heart);
void ImageStroke(uint16_t strokes);
void ImageSPLIT(uint16_t split);
void buffToImage(void);
void ImageSPM_LEVEL(uint16_t spmlevel);
void ImageSPM_TARGET(uint16_t target);
void ImageRsistanceLevel(uint16_t n);
void ImageDrag(uint16_t drag);
void ImageGraph(uint16_t drag[]);
void ImagePaddle(uint16_t rpm, uint16_t level);
void ImagePaddleAll(uint16_t level);
void dis_MANUAL(void);
void dis_TARGET(void);
void dis_TIME(void);
void dis_DISTANCE(void);
void dis_CALORIES(void);
void dis_STROKES(void);
void dis_SPM(uint8_t n);
void dis_BT(void);
void dis_INTERVAL(uint8_t n);
void dis_INTERVAL_1(uint16_t m, uint16_t n, uint16_t s);
void dis_HISTORY(uint8_t n);
void dis_PAUSE(void);
void dis_STOP(void);
#endif                                                            /* IMAGE_H */
