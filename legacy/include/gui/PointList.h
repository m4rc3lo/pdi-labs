/* 
 * File:   PointList.h
 * Author: marcelo
 *
 * Created on 22 de Agosto de 2016, 14:00
 */

#ifndef _POINTLIST_H
#define	_POINTLIST_H

#include "ui_PointList.h"

class PointList : public QDialog 
{
    Q_OBJECT
    public:
        
        PointList();
        virtual ~PointList();
        
    private:
    
        Ui::PointList widget;
};

#endif	/* _POINTLIST_H */
