/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/


#ifndef BERRYCOMPATIBILITYACTIVATOR_P_H
#define BERRYCOMPATIBILITYACTIVATOR_P_H

#include <ctkPluginActivator.h>

namespace berry {

class CTKPluginListener;

class CompatibilityActivator :
  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)

public:

  CompatibilityActivator();
  ~CompatibilityActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:

  CTKPluginListener* pluginListener;

}; // CompatibilityActivator

}

#endif // BERRYCOMPATIBILITYACTIVATOR_P_H
