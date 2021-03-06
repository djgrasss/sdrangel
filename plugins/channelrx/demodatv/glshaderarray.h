///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2017 F4HKW                                                      //
// for F4EXB / SDRAngel                                                          //
//                                                                               //
// This program is free software; you can redistribute it and/or modify          //
// it under the terms of the GNU General Public License as published by          //
// the Free Software Foundation as version 3 of the License, or                  //
//                                                                               //
// This program is distributed in the hope that it will be useful,               //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  //
// GNU General Public License V3 for more details.                               //
//                                                                               //
// You should have received a copy of the GNU General Public License             //
// along with this program. If not, see <http://www.gnu.org/licenses/>.          //
///////////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDE_GUI_GLSHADERARRAY_H_
#define INCLUDE_GUI_GLSHADERARRAY_H_

#include <QString>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_2_0>
#include <QOpenGLFunctions_2_1>
#include <QOpenGLFunctions_3_0>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLContext>
#include <QMatrix4x4>
#include <QVector4D>
#include <QDebug>
#include <QColor>
#include <math.h>

class QOpenGLShaderProgram;
class QMatrix4x4;
class QVector4D;

class GLShaderArray
{
public:
    GLShaderArray();
    ~GLShaderArray();

    void InitializeGL(int intCols, int intRows);
    void ResizeContainer(int intCols, int intRows);
    void Cleanup();
    QRgb *GetRowBuffer(int intRow);
    void RenderPixels(unsigned char *chrData);
    void ResetPixels();

    bool SelectRow(int intLine);
    bool SetDataColor(int intCol,QRgb objColor);


protected:

    QOpenGLShaderProgram *m_objProgram;
    int m_objMatrixLoc;
    int m_objTextureLoc;
    int m_objColorLoc;
    static const QString m_strVertexShaderSourceArray;
    static const QString m_strFragmentShaderSourceColored;

    QImage *m_objImage=NULL;   
    QOpenGLTexture *m_objTexture=NULL;

    int m_intCols;
    int m_intRows;

    QRgb * m_objCurrentRow;

    bool m_blnInitialized;
};

#endif /* INCLUDE_GUI_GLSHADERARRAY_H_ */
