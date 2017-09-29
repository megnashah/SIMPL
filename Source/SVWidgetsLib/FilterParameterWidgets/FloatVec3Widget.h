/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _floatvec3widget_h_
#define _floatvec3widget_h_


#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtWidgets/QWidget>

#include "SVWidgetsLib/QtSupport/QtSFaderWidget.h"

#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"


#include "SVWidgetsLib/ui_FloatVec3Widget.h"


/**
* @brief
* @author
* @version
*/
class SVWidgetsLib_EXPORT FloatVec3Widget : public FilterParameterWidget, private Ui::FloatVec3Widget
{
    Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    FloatVec3Widget(FilterParameter* parameter, AbstractFilter* filter = nullptr, QWidget* parent = nullptr);

    virtual ~FloatVec3Widget();

    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:
    void widgetChanged(const QString& msg);
    void filterNeedsInputParameters(AbstractFilter* filter); // When the filter is ready for us to update its input parameter(s) that we are responsible for
    void beforePreflight(); // Called just before the "dataCheck()" is called
    void afterPreflight(); // Called just after the dataCheck() is called.


  signals:
    void errorSettingFilterParameter(const QString& msg);
    void parametersChanged();

  private:

    bool m_DidCausePreflight;

    FloatVec3FilterParameter* m_FilterParameter;


    FloatVec3Widget(const FloatVec3Widget&); // Copy Constructor Not Implemented
    void operator=(const FloatVec3Widget&); // Operator '=' Not Implemented

};

#endif /* _FloatVec3Widget_H_ */


