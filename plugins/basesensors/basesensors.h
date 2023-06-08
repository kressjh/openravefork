// Copyright (C) 2006-2008 Rosen Diankov (rdiankov@cs.cmu.edu)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
#ifndef OPENRAVE_PLUGIN_BASESENSORS_H
#define OPENRAVE_PLUGIN_BASESENSORS_H

#include <openrave/openrave.h>
#include <openrave/plugin.h>

struct BaseSensorsPlugin : public RavePlugin {
    BaseSensorsPlugin();
    ~BaseSensorsPlugin() override;

    OpenRAVE::InterfaceBasePtr CreateInterface(OpenRAVE::InterfaceType type, const std::string& interfacename, std::istream& sinput, OpenRAVE::EnvironmentBasePtr penv) override;
    const InterfaceMap& GetInterfaces() const override;
    const std::string& GetPluginName() const override;

private:
    static const std::string _pluginname;
    InterfaceMap _interfaces;
    std::list<OpenRAVE::UserDataPtr> s_listRegisteredReaders;
};

#endif // OPENRAVE_PLUGIN_BASESENSORS_H