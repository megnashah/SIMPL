#pragma once

#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "PythonBindingsModule.h"
/* *****************************************************************************
 *
 *
 ******************************************************************************/
class PyBind11Generator
{

public:
  /**
 * @brief PyBind11Generator
 * @param topLevelDir
 * @param charsToStrip
 * @param libName
 */
  PyBind11Generator(const QDir& topLevelDir, const QString& charsToStrip, const QString& libName, 
                   const QString& genDir, const QString& moduleTemplatePath, const QString& isSIMPLib, const QString& cfgIntDir);

  ~PyBind11Generator();

  /**
 * @brief execute
 */
  void execute();

  /**
 * @brief recursiveSearch
 * @param currentDir
 */
  void recursiveSearch(const QDir& currentDir);

  /**
 * @brief generatePybind11Header
 * @param hFile
 */
  void generatePybind11Header(const QString& hFile);

  /**
   * @brief
   */
  void copyPyInitFiles();

  /**
   * 
   */
  void generateModuleInitPy();

  /**
  *
  */
  void readFilterList();
  
private:
  QDir m_TopLevelDir;
  QDir m_SourceDir;
  QString m_CharsToStrip;
  QString m_LibName;
  QString m_LibNameUpper;
  QString m_GenDir;
  QString m_ModuleTemplatePath;
  PythonBindingsModule m_ModuleCode;
  QString m_IsSIMPLib;
  QString m_CfgIntDir;
  QStringList m_FilterList;
  int m_FileGenCount = 0;
  QString m_DiagnosticMessage;

  PyBind11Generator(const PyBind11Generator&) = delete; // Copy Constructor Not Implemented
  void operator=(const PyBind11Generator&) = delete;    // Operator '=' Not Implemented
};
