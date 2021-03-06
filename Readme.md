TwinPad Plugin
=
TwinPad is an input plugin for PSX (PSEmu Pro compliant plugin API) and a PCSX2 emulators. Based on the original work of Linuzappz and asadr back in 2004. It has GPLv2 or v3 license as the original plugin. Currently it supports only Windows platform from Windows XP to Windows 10. For now it supports Keyboard and Mouse using DirectInput 8.

The plugin features a unique function to record macros and replay them when needed. I call this feature Combo, please see the official PCSX2 forum page for details on usage [here](http://forums.pcsx2.net/Thread-TwinPad-v0-9-2)

---
### Dependencies
TwinPad uses wxWidgets library, and it uses the latest development build of
wxWidgets v3.1.0, You **must** use a version of wxWidgets starting from this commit [35f35ea](https://github.com/wxWidgets/wxWidgets/commit/35f35ea407d602f73c5fbe38a4bf6cc243ac3368) (or [64e14e8](https://github.com/wxWidgets/wxWidgets/commit/64e14e850d9096ebc83dd077569b1c0b7862c737) if you are using wxWidgets without precompiled headers). That first commit contains a fix to wxToolTip behavior under Windows, and it is crucial for TwinPad to work correctly.

>**Note**: For more details about wxToolTip bug that was fixed at the aforementioned commit, [click here](http://trac.wxwidgets.org/ticket/17360). 

Before you compile wxWidgets library, there are some modifications need to
be done beforehand.

Now we need to setup wxWidgets and tell Visual Studio how to compile it:
- Open `setup.h` from `_custom_build` project under `Setup Headers` and change the defined `wxUSE_COMPILER_TLS` value from 1 to 2.
- Select and highlight all projects from Solution Explorer and click properties. Choose `Debug` configuration and change the platform toolset to use **Visual Studio 2015 - Windows XP (v140_XP)**. Click apply then change the configuration to `Release` and do the same. Then click OK.
- Select and highlight all projects except for `_custom_build`, right-click then properties. Go to `C/C++` section and choose `Command Line` and under Additional Options, put `/Zc:threadSafeInit-` for both Debug and Release configurations. **Note:** This compiler option is also included in TwinPad project in the Common property sheet.
- Build the Solution once for Debug, and another for Release.

**Note:** This compiler keyword was introduced in Visual Studio 2014 CTP to fix the buggy and remaining TLS issues in Windows XP, since it is out of support, it can't be fixed. Newer versions of Windows (Vista +) don't have these issues. For more information see this [bug report](https://connect.microsoft.com/VisualStudio/feedback/details/1789709/visual-c-2015-runtime-broken-on-windows-server-2003-c-11-magic-statics), or [here](http://www.btday.com/access-violation-on-static-initialization/) for the same issue but with code.

> **Note:** If you are using Visual Studio 2013, there is no need for this. Except you might have to change the platform toolset to use **Visual Studio 2013 - Windows XP (v120_XP)** instead. For both TwinPad and wxWidgets. This is necessary to make TwinPad work under Windows XP! The plugin will throw Access Denied with error 998 for inaccessible memory location. wxWidgets have an issue with Thread Local Storage (TLS) when the library is used in a DLL (Dynamic Link Library) as you might have read in the `setup.h` file when changing wxUSE_COMPILER_TLS value. See this bug [ticket](trac.wxwidgets.org/ticket/13116). They fixed it for earlier versions of Visual Studio (v11) and when using Windows SDK v7.1, but it remains for newer Visual Studio v14 (2015) and newer Windows SDK v8.1

> **Note 2:** If you are using wxWidgets development build downloaded via github terminal window, you might not have `setup.h` file. You have to copy `setup0.h` and paste then rename it to `setup.h`. The file is found in `\wxWidgets\include\wx\msw` For more information, read the `install.txt` file included in wxWidgets for Microsoft Windows (MSW).

One last thing, almost done. You need to set Environment Variable for Windows so that Visual Studio can see wxWidgets directory using **$(WXWIN)** variable. By doing this, it makes wxWidgets installation portable to other computers or even yours if you decided to change the directory of wxWidgets, and you don't have to modify your project files to accommodate.

- Right click My Computer/This PC and select properties, or go to Control Panel and run System tool from there.
- Select Advanced System Options and then Environmental Variables.
-  Under System Variables, create a new one and choose `WXWIN` for the Variable Name and its Value the directory which wxWidgets is installed in (e.g, `D:\wxWidgets`).
- Depending on the version of Windows, you might have to Log off or Restart the computer for the Environment Variable to be read by Visual Studio.

----
### License
> Copyright (C) 2002-2004  PADwin Team
> Copyright (C) 2006-2016 Rebel_X
> 
> This program is free software; you can redistribute it and/or modify
> it under the terms of the GNU General Public License as published by
> the Free Software Foundation; either version 2 of the License, or
> (at your option) any later version.
> 
> This program is distributed in the hope that it will be useful,
> but WITHOUT ANY WARRANTY; without even the implied warranty of
> MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
> GNU General Public License for more details.
> 
> You should have received a copy of the GNU General Public License
> along with this program; if not, write to the Free Software 
> Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
> 
> THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY 
> EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
> IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
> A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
> AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
> EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
> NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
> LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
> CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
> STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
> ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
> ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
