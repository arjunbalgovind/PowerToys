﻿// Copyright (c) Microsoft Corporation
// The Microsoft Corporation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using System;
using System.Collections.Generic;
using System.Linq;
using Moq;
using NUnit.Framework;
using Wox.Infrastructure;
using Wox.Infrastructure.FileSystemHelper;
using Wox.Plugin;

namespace Microsoft.Plugin.Program.UnitTests.Programs
{
    using Win32Program = Microsoft.Plugin.Program.Programs.Win32Program;

    [TestFixture]
    public class Win32Tests
    {
        private static readonly Win32Program _notepadAppdata = new Win32Program
        {
            Name = "Notepad",
            ExecutableName = "notepad.exe",
            FullPath = "c:\\windows\\system32\\notepad.exe",
            LnkResolvedPath = "c:\\users\\powertoys\\appdata\\roaming\\microsoft\\windows\\start menu\\programs\\accessories\\notepad.lnk",
            AppType = Win32Program.ApplicationType.Win32Application,
        };

        private static readonly Win32Program _notepadUsers = new Win32Program
        {
            Name = "Notepad",
            ExecutableName = "notepad.exe",
            FullPath = "c:\\windows\\system32\\notepad.exe",
            LnkResolvedPath = "c:\\programdata\\microsoft\\windows\\start menu\\programs\\accessories\\notepad.lnk",
            AppType = Win32Program.ApplicationType.Win32Application,
        };

        private static readonly Win32Program _azureCommandPrompt = new Win32Program
        {
            Name = "Microsoft Azure Command Prompt - v2.9",
            ExecutableName = "cmd.exe",
            FullPath = "c:\\windows\\system32\\cmd.exe",
            LnkResolvedPath = "c:\\programdata\\microsoft\\windows\\start menu\\programs\\microsoft azure\\microsoft azure sdk for .net\\v2.9\\microsoft azure command prompt - v2.9.lnk",
            AppType = Win32Program.ApplicationType.Win32Application,
        };

        private static readonly Win32Program _visualStudioCommandPrompt = new Win32Program
        {
            Name = "x64 Native Tools Command Prompt for VS 2019",
            ExecutableName = "cmd.exe",
            FullPath = "c:\\windows\\system32\\cmd.exe",
            LnkResolvedPath = "c:\\programdata\\microsoft\\windows\\start menu\\programs\\visual studio 2019\\visual studio tools\\vc\\x64 native tools command prompt for vs 2019.lnk",
            AppType = Win32Program.ApplicationType.Win32Application,
        };

        private static readonly Win32Program _commandPrompt = new Win32Program
        {
            Name = "Command Prompt",
            ExecutableName = "cmd.exe",
            FullPath = "c:\\windows\\system32\\cmd.exe",
            LnkResolvedPath = "c:\\users\\powertoys\\appdata\\roaming\\microsoft\\windows\\start menu\\programs\\system tools\\command prompt.lnk",
            AppType = Win32Program.ApplicationType.Win32Application,
        };

        private static readonly Win32Program _fileExplorer = new Win32Program
        {
            Name = "File Explorer",
            ExecutableName = "File Explorer.lnk",
            FullPath = "c:\\users\\powertoys\\appdata\\roaming\\microsoft\\windows\\start menu\\programs\\system tools\\file explorer.lnk",
            LnkResolvedPath = null,
            AppType = Win32Program.ApplicationType.Win32Application,
        };

        private static readonly Win32Program _wordpad = new Win32Program
        {
            Name = "Wordpad",
            ExecutableName = "wordpad.exe",
            FullPath = "c:\\program files\\windows nt\\accessories\\wordpad.exe",
            LnkResolvedPath = "c:\\programdata\\microsoft\\windows\\start menu\\programs\\accessories\\wordpad.lnk",
            AppType = Win32Program.ApplicationType.Win32Application,
        };

        private static readonly Win32Program _wordpadDuplicate = new Win32Program
        {
            Name = "WORDPAD",
            ExecutableName = "WORDPAD.EXE",
            FullPath = "c:\\program files\\windows nt\\accessories\\wordpad.exe",
            LnkResolvedPath = null,
            AppType = Win32Program.ApplicationType.Win32Application,
        };

        private static readonly Win32Program _twitterChromePwa = new Win32Program
        {
            Name = "Twitter",
            FullPath = "c:\\program files (x86)\\google\\chrome\\application\\chrome_proxy.exe",
            LnkResolvedPath = "c:\\users\\powertoys\\appdata\\roaming\\microsoft\\windows\\start menu\\programs\\chrome apps\\twitter.lnk",
            Arguments = " --profile-directory=Default --app-id=jgeosdfsdsgmkedfgdfgdfgbkmhcgcflmi",
            AppType = 0,
        };

        private static readonly Win32Program _pinnedWebpage = new Win32Program
        {
            Name = "Web page",
            FullPath = "c:\\program files (x86)\\microsoft\\edge\\application\\msedge_proxy.exe",
            LnkResolvedPath = "c:\\users\\powertoys\\appdata\\roaming\\microsoft\\windows\\start menu\\programs\\web page.lnk",
            Arguments = "--profile-directory=Default --app-id=homljgmgpmcbpjbnjpfijnhipfkiclkd",
            AppType = 0,
        };

        private static readonly Win32Program _edgeNamedPinnedWebpage = new Win32Program
        {
            Name = "edge - Bing",
            FullPath = "c:\\program files (x86)\\microsoft\\edge\\application\\msedge_proxy.exe",
            LnkResolvedPath = "c:\\users\\powertoys\\appdata\\roaming\\microsoft\\windows\\start menu\\programs\\edge - bing.lnk",
            Arguments = "  --profile-directory=Default --app-id=aocfnapldcnfbofgmbbllojgocaelgdd",
            AppType = 0,
        };

        private static readonly Win32Program _msedge = new Win32Program
        {
            Name = "Microsoft Edge",
            ExecutableName = "msedge.exe",
            FullPath = "c:\\program files (x86)\\microsoft\\edge\\application\\msedge.exe",
            LnkResolvedPath = "c:\\programdata\\microsoft\\windows\\start menu\\programs\\microsoft edge.lnk",
            AppType = Win32Program.ApplicationType.Win32Application,
        };

        private static readonly Win32Program _chrome = new Win32Program
        {
            Name = "Google Chrome",
            ExecutableName = "chrome.exe",
            FullPath = "c:\\program files (x86)\\google\\chrome\\application\\chrome.exe",
            LnkResolvedPath = "c:\\programdata\\microsoft\\windows\\start menu\\programs\\google chrome.lnk",
            AppType = Win32Program.ApplicationType.Win32Application,
        };

        private static readonly Win32Program _dummyProxyApp = new Win32Program
        {
            Name = "Proxy App",
            ExecutableName = "test_proxy.exe",
            FullPath = "c:\\program files (x86)\\microsoft\\edge\\application\\test_proxy.exe",
            LnkResolvedPath = "c:\\programdata\\microsoft\\windows\\start menu\\programs\\test proxy.lnk",
            AppType = Win32Program.ApplicationType.Win32Application,
        };

        private static readonly Win32Program _cmdRunCommand = new Win32Program
        {
            Name = "cmd",
            ExecutableName = "cmd.exe",
            FullPath = "c:\\windows\\system32\\cmd.exe",
            LnkResolvedPath = null,
            AppType = Win32Program.ApplicationType.RunCommand, // Run command
        };

        private static readonly Win32Program _cmderRunCommand = new Win32Program
        {
            Name = "Cmder",
            Description = "Cmder: Lovely Console Emulator",
            ExecutableName = "Cmder.exe",
            FullPath = "c:\\tools\\cmder\\cmder.exe",
            LnkResolvedPath = null,
            AppType = Win32Program.ApplicationType.RunCommand, // Run command
        };

        private static readonly Win32Program _dummyInternetShortcutApp = new Win32Program
        {
            Name = "Shop Titans",
            ExecutableName = "Shop Titans.url",
            FullPath = "steam://rungameid/1258080",
            ParentDirectory = "C:\\Users\\temp\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Steam",
            LnkResolvedPath = null,
            AppType = Win32Program.ApplicationType.InternetShortcutApplication,
        };

        private static readonly Win32Program _dummyInternetShortcutAppDuplicate = new Win32Program
        {
            Name = "Shop Titans",
            ExecutableName = "Shop Titans.url",
            FullPath = "steam://rungameid/1258080",
            ParentDirectory = "C:\\Users\\temp\\Desktop",
            LnkResolvedPath = null,
            AppType = Win32Program.ApplicationType.InternetShortcutApplication,
        };

        private static readonly Win32Program _dummyAppRefApp = new Win32Program
        {
            Name = "Dummy AppRef Application",
            ExecutableName = "dummy.appref-ms",
            FullPath = "C:\\dummy.appref-ms",
            ParentDirectory = "C:\\",
            LnkResolvedPath = null,
            AppType = Win32Program.ApplicationType.ApprefApplication,
        };

        private static readonly Win32Program _dummyShortcutApp = new Win32Program
        {
            Name = "Dummy Shortcut Application",
            ExecutableName = "application.lnk",
            FullPath = "C:\\application.lnk",
            ParentDirectory = "C:\\",
            LnkResolvedPath = "C:\\application.lnk",
            AppType = Win32Program.ApplicationType.ShortcutApplication,
        };

        private static readonly Win32Program _dummyFolderApp = new Win32Program
        {
            Name = "Dummy Folder",
            ExecutableName = "application.lnk",
            FullPath = "C:\\dummy\\folder",
            ParentDirectory = "C:\\dummy\\",
            LnkResolvedPath = "C:\\tools\\application.lnk",
            AppType = Win32Program.ApplicationType.Folder,
        };

        private static readonly Win32Program _dummyGenericFileApp = new Win32Program
        {
            Name = "Dummy Folder",
            ExecutableName = "application.lnk",
            FullPath = "C:\\dummy\\file.pdf",
            ParentDirectory = "C:\\dummy\\",
            LnkResolvedPath = "C:\\tools\\application.lnk",
            AppType = Win32Program.ApplicationType.GenericFile,
        };

        private static IDirectoryWrapper GetMockedDirectoryWrapper()
        {
            var mockDirectory = new Mock<IDirectoryWrapper>();

            // Check if the file has no extension. This is not actually true since there can be files without extensions, but this is sufficient for the purpose of a mock function
            Func<string, bool> returnValue = arg => string.IsNullOrEmpty(System.IO.Path.GetExtension(arg));
            mockDirectory.Setup(m => m.Exists(It.IsAny<string>())).Returns(returnValue);
            return mockDirectory.Object;
        }

        [Test]
        public void DedupFunctionWhenCalledMustRemoveDuplicateNotepads()
        {
            // Arrange
            List<Win32Program> prgms = new List<Win32Program>
            {
                _notepadAppdata,
                _notepadUsers,
            };

            // Act
            Win32Program[] apps = Win32Program.DeduplicatePrograms(prgms.AsParallel());

            // Assert
            Assert.AreEqual(apps.Length, 1);
        }

        [Test]
        public void DedupFunctionWhenCalledMustRemoveInternetShortcuts()
        {
            // Arrange
            List<Win32Program> prgms = new List<Win32Program>
            {
                _dummyInternetShortcutApp,
                _dummyInternetShortcutAppDuplicate,
            };

            // Act
            Win32Program[] apps = Win32Program.DeduplicatePrograms(prgms.AsParallel());

            // Assert
            Assert.AreEqual(apps.Length, 1);
        }

        [Test]
        public void DedupFunctionWhenCalledMustNotRemovelnkWhichdoesNotHaveExe()
        {
            // Arrange
            List<Win32Program> prgms = new List<Win32Program>
            {
                _fileExplorer,
            };

            // Act
            Win32Program[] apps = Win32Program.DeduplicatePrograms(prgms.AsParallel());

            // Assert
            Assert.AreEqual(apps.Length, 1);
        }

        [Test]
        public void DedupFunctionMustRemoveDuplicatesForExeExtensionsWithoutLnkResolvedPath()
        {
            // Arrange
            List<Win32Program> prgms = new List<Win32Program>
            {
                _wordpad,
                _wordpadDuplicate,
            };

            // Act
            Win32Program[] apps = Win32Program.DeduplicatePrograms(prgms.AsParallel());

            // Assert
            Assert.AreEqual(apps.Length, 1);
            Assert.IsTrue(!string.IsNullOrEmpty(apps[0].LnkResolvedPath));
        }

        [Test]
        public void DedupFunctionMustNotRemoveProgramsWithSameExeNameAndFullPath()
        {
            // Arrange
            List<Win32Program> prgms = new List<Win32Program>
            {
                _azureCommandPrompt,
                _visualStudioCommandPrompt,
                _commandPrompt,
            };

            // Act
            Win32Program[] apps = Win32Program.DeduplicatePrograms(prgms.AsParallel());

            // Assert
            Assert.AreEqual(apps.Length, 3);
        }

        [Test]
        public void FunctionIsWebApplicationShouldReturnTrueForWebApplications()
        {
            // The IsWebApplication(() function must return true for all PWAs and pinned web pages
            Assert.IsTrue(_twitterChromePwa.IsWebApplication());
            Assert.IsTrue(_pinnedWebpage.IsWebApplication());
            Assert.IsTrue(_edgeNamedPinnedWebpage.IsWebApplication());

            // Should not filter apps whose executable name ends with proxy.exe
            Assert.IsFalse(_dummyProxyApp.IsWebApplication());
        }

        [TestCase("ignore")]
        public void FunctionFilterWebApplicationShouldReturnFalseWhenSearchingForTheMainApp(string query)
        {
            // Irrespective of the query, the FilterWebApplication() Function must not filter main apps such as edge and chrome
            Assert.IsFalse(_msedge.FilterWebApplication(query));
            Assert.IsFalse(_chrome.FilterWebApplication(query));
        }

        [TestCase("edge", ExpectedResult = true)]
        [TestCase("EDGE", ExpectedResult = true)]
        [TestCase("msedge", ExpectedResult = true)]
        [TestCase("Microsoft", ExpectedResult = true)]
        [TestCase("edg", ExpectedResult = true)]
        [TestCase("Edge page", ExpectedResult = false)]
        [TestCase("Edge Web page", ExpectedResult = false)]
        public bool EdgeWebSitesShouldBeFilteredWhenSearchingForEdge(string query)
        {
            return _pinnedWebpage.FilterWebApplication(query);
        }

        [TestCase("chrome", ExpectedResult = true)]
        [TestCase("CHROME", ExpectedResult = true)]
        [TestCase("Google", ExpectedResult = true)]
        [TestCase("Google Chrome", ExpectedResult = true)]
        [TestCase("Google Chrome twitter", ExpectedResult = false)]
        public bool ChromeWebSitesShouldBeFilteredWhenSearchingForChrome(string query)
        {
            return _twitterChromePwa.FilterWebApplication(query);
        }

        [TestCase("twitter", 0, ExpectedResult = false)]
        [TestCase("Twit", 0, ExpectedResult = false)]
        [TestCase("TWITTER", 0, ExpectedResult = false)]
        [TestCase("web", 1, ExpectedResult = false)]
        [TestCase("Page", 1, ExpectedResult = false)]
        [TestCase("WEB PAGE", 1, ExpectedResult = false)]
        [TestCase("edge", 2, ExpectedResult = false)]
        [TestCase("EDGE", 2, ExpectedResult = false)]
        public bool PinnedWebPagesShouldNotBeFilteredWhenSearchingForThem(string query, int scenario)
        {
            const int CASE_TWITTER = 0;
            const int CASE_WEB_PAGE = 1;
            const int CASE_EDGE_NAMED_WEBPAGE = 2;

            // If the query is a part of the name of the web application, it should not be filtered,
            // even if the name is the same as that of the main application, eg: case 2 - edge
            switch (scenario)
            {
                case CASE_TWITTER:
                    return _twitterChromePwa.FilterWebApplication(query);
                case CASE_WEB_PAGE:
                    return _pinnedWebpage.FilterWebApplication(query);
                case CASE_EDGE_NAMED_WEBPAGE:
                    return _edgeNamedPinnedWebpage.FilterWebApplication(query);
                default:
                    break;
            }

            // unreachable code
            return true;
        }

        [TestCase("Command Prompt")]
        [TestCase("cmd")]
        [TestCase("cmd.exe")]
        [TestCase("ignoreQueryText")]
        public void Win32ApplicationsShouldNotBeFilteredWhenFilteringRunCommands(string query)
        {
            // Even if there is an exact match in the name or exe name, win32 applications should never be filtered
            Assert.IsTrue(_commandPrompt.QueryEqualsNameForRunCommands(query));
        }

        [TestCase("cmd")]
        [TestCase("Cmd")]
        [TestCase("CMD")]
        public void RunCommandsShouldNotBeFilteredOnExactMatch(string query)
        {
            // Partial matches should be filtered as cmd is not equal to cmder
            Assert.IsFalse(_cmderRunCommand.QueryEqualsNameForRunCommands(query));

            // the query matches the name (cmd) and is therefore not filtered (case-insensitive)
            Assert.IsTrue(_cmdRunCommand.QueryEqualsNameForRunCommands(query));
        }

        [Test]
        public void WebApplicationShouldReturnContextMenuWithOpenInConsoleWhenContextMenusIsCalled()
        {
            // Arrange
            var mock = new Mock<IPublicAPI>();

            // Act
            List<ContextMenuResult> contextMenuResults = _pinnedWebpage.ContextMenus(mock.Object);

            // Assert
            Assert.AreEqual(contextMenuResults.Count, 3);
            Assert.AreEqual(contextMenuResults[0].Title, Properties.Resources.wox_plugin_program_run_as_administrator);
            Assert.AreEqual(contextMenuResults[1].Title, Properties.Resources.wox_plugin_program_open_containing_folder);
            Assert.AreEqual(contextMenuResults[2].Title, Properties.Resources.wox_plugin_program_open_in_console);
        }

        [Test]
        public void InternetShortcutApplicationShouldReturnContextMenuWithOpenInConsoleWhenContextMenusIsCalled()
        {
            // Arrange
            var mock = new Mock<IPublicAPI>();

            // Act
            List<ContextMenuResult> contextMenuResults = _dummyInternetShortcutApp.ContextMenus(mock.Object);

            // Assert
            Assert.AreEqual(contextMenuResults.Count, 2);
            Assert.AreEqual(contextMenuResults[0].Title, Properties.Resources.wox_plugin_program_open_containing_folder);
            Assert.AreEqual(contextMenuResults[1].Title, Properties.Resources.wox_plugin_program_open_in_console);
        }

        [Test]
        public void Win32ApplicationShouldReturnContextMenuWithOpenInConsoleWhenContextMenusIsCalled()
        {
            // Arrange
            var mock = new Mock<IPublicAPI>();

            // Act
            List<ContextMenuResult> contextMenuResults = _chrome.ContextMenus(mock.Object);

            // Assert
            Assert.AreEqual(contextMenuResults.Count, 3);
            Assert.AreEqual(contextMenuResults[0].Title, Properties.Resources.wox_plugin_program_run_as_administrator);
            Assert.AreEqual(contextMenuResults[1].Title, Properties.Resources.wox_plugin_program_open_containing_folder);
            Assert.AreEqual(contextMenuResults[2].Title, Properties.Resources.wox_plugin_program_open_in_console);
        }

        [Test]
        public void RunCommandShouldReturnContextMenuWithOpenInConsoleWhenContextMenusIsCalled()
        {
            // Arrange
            var mock = new Mock<IPublicAPI>();

            // Act
            List<ContextMenuResult> contextMenuResults = _cmdRunCommand.ContextMenus(mock.Object);

            // Assert
            Assert.AreEqual(contextMenuResults.Count, 3);
            Assert.AreEqual(contextMenuResults[0].Title, Properties.Resources.wox_plugin_program_run_as_administrator);
            Assert.AreEqual(contextMenuResults[1].Title, Properties.Resources.wox_plugin_program_open_containing_folder);
            Assert.AreEqual(contextMenuResults[2].Title, Properties.Resources.wox_plugin_program_open_in_console);
        }

        [Test]
        public void AppRefApplicationShouldReturnContextMenuWithOpenInConsoleWhenContextMenusIsCalled()
        {
            // Arrange
            var mock = new Mock<IPublicAPI>();

            // Act
            List<ContextMenuResult> contextMenuResults = _dummyAppRefApp.ContextMenus(mock.Object);

            // Assert
            Assert.AreEqual(contextMenuResults.Count, 3);
            Assert.AreEqual(contextMenuResults[0].Title, Properties.Resources.wox_plugin_program_run_as_administrator);
            Assert.AreEqual(contextMenuResults[1].Title, Properties.Resources.wox_plugin_program_open_containing_folder);
            Assert.AreEqual(contextMenuResults[2].Title, Properties.Resources.wox_plugin_program_open_in_console);
        }

        [Test]
        public void ShortcutApplicationShouldReturnContextMenuWithOpenInConsoleWhenContextMenusIsCalled()
        {
            // Arrange
            var mock = new Mock<IPublicAPI>();

            // Act
            List<ContextMenuResult> contextMenuResults = _dummyShortcutApp.ContextMenus(mock.Object);

            // Assert
            Assert.AreEqual(contextMenuResults.Count, 3);
            Assert.AreEqual(contextMenuResults[0].Title, Properties.Resources.wox_plugin_program_run_as_administrator);
            Assert.AreEqual(contextMenuResults[1].Title, Properties.Resources.wox_plugin_program_open_containing_folder);
            Assert.AreEqual(contextMenuResults[2].Title, Properties.Resources.wox_plugin_program_open_in_console);
        }

        [Test]
        public void FolderApplicationShouldReturnContextMenuWithOpenInConsoleWhenContextMenusIsCalled()
        {
            // Arrange
            var mock = new Mock<IPublicAPI>();

            // Act
            List<ContextMenuResult> contextMenuResults = _dummyFolderApp.ContextMenus(mock.Object);

            // Assert
            Assert.AreEqual(contextMenuResults.Count, 2);
            Assert.AreEqual(contextMenuResults[0].Title, Properties.Resources.wox_plugin_program_open_containing_folder);
            Assert.AreEqual(contextMenuResults[1].Title, Properties.Resources.wox_plugin_program_open_in_console);
        }

        [Test]
        public void GenericFileApplicationShouldReturnContextMenuWithOpenInConsoleWhenContextMenusIsCalled()
        {
            // Arrange
            var mock = new Mock<IPublicAPI>();

            // Act
            List<ContextMenuResult> contextMenuResults = _dummyGenericFileApp.ContextMenus(mock.Object);

            // Assert
            Assert.AreEqual(contextMenuResults.Count, 2);
            Assert.AreEqual(contextMenuResults[0].Title, Properties.Resources.wox_plugin_program_open_containing_folder);
            Assert.AreEqual(contextMenuResults[1].Title, Properties.Resources.wox_plugin_program_open_in_console);
        }

        [Test]
        public void Win32AppsShouldSetNameAsTitleWhileCreatingResult()
        {
            var mock = new Mock<IPublicAPI>();
            StringMatcher.Instance = new StringMatcher();

            // Act
            var result = _cmderRunCommand.Result("cmder", mock.Object);

            // Assert
            Assert.IsTrue(result.Title.Equals(_cmderRunCommand.Name, StringComparison.Ordinal));
            Assert.IsFalse(result.Title.Equals(_cmderRunCommand.Description, StringComparison.Ordinal));
        }

        [Test]
        public void GetAppTypeFromPathShouldReturnWin32ApplicationWhenExeFilePathIsPassedAsArgument()
        {
            // Arrange
            string path = "C:\\Program Files\\dummy.exe";

            // Directory.Exists must be mocked
            Win32Program.DirectoryWrapper = GetMockedDirectoryWrapper();

            // Act
            Win32Program.ApplicationType appType = Win32Program.GetAppTypeFromPath(path);

            // Assert
            Assert.AreEqual(appType, Win32Program.ApplicationType.Win32Application);
        }

        [Test]
        public void GetAppTypeFromPathShouldReturnWin32ApplicationWhenMscFilePathIsPassedAsArgument()
        {
            // Arrange
            string path = "C:\\Program Files\\dummy.msc";

            // Directory.Exists must be mocked
            Win32Program.DirectoryWrapper = GetMockedDirectoryWrapper();

            // Act
            Win32Program.ApplicationType appType = Win32Program.GetAppTypeFromPath(path);

            // Assert
            Assert.AreEqual(appType, Win32Program.ApplicationType.Win32Application);
        }

        [Test]
        public void GetAppTypeFromPathShouldReturnShortcutApplicationWhenLnkFilePathIsPassedAsArgument()
        {
            // Arrange
            string path = "C:\\Program Files\\dummy.lnk";

            // Directory.Exists must be mocked
            Win32Program.DirectoryWrapper = GetMockedDirectoryWrapper();

            // Act
            Win32Program.ApplicationType appType = Win32Program.GetAppTypeFromPath(path);

            // Assert
            Assert.AreEqual(appType, Win32Program.ApplicationType.ShortcutApplication);
        }

        [Test]
        public void GetAppTypeFromPathShouldReturnApprefApplicationWhenApprefMsFilePathIsPassedAsArgument()
        {
            // Arrange
            string path = "C:\\Program Files\\dummy.appref-ms";

            // Directory.Exists must be mocked
            Win32Program.DirectoryWrapper = GetMockedDirectoryWrapper();

            // Act
            Win32Program.ApplicationType appType = Win32Program.GetAppTypeFromPath(path);

            // Assert
            Assert.AreEqual(appType, Win32Program.ApplicationType.ApprefApplication);
        }

        [Test]
        public void GetAppTypeFromPathShouldReturnInternetShortcutApplicationWhenUrlFilePathIsPassedAsArgument()
        {
            // Arrange
            string path = "C:\\Program Files\\dummy.url";

            // Directory.Exists must be mocked
            Win32Program.DirectoryWrapper = GetMockedDirectoryWrapper();

            // Act
            Win32Program.ApplicationType appType = Win32Program.GetAppTypeFromPath(path);

            // Assert
            Assert.AreEqual(appType, Win32Program.ApplicationType.InternetShortcutApplication);
        }

        [Test]
        public void GetAppTypeFromPathShouldReturnFolderWhenFolderPathIsPassedAsArgument()
        {
            // Arrange
            string path = "C:\\Program Files\\dummy";

            // Directory.Exists must be mocked
            Win32Program.DirectoryWrapper = GetMockedDirectoryWrapper();

            // Act
            Win32Program.ApplicationType appType = Win32Program.GetAppTypeFromPath(path);

            // Assert
            Assert.AreEqual(appType, Win32Program.ApplicationType.Folder);
        }

        [Test]
        public void GetAppTypeFromPathShouldReturnGenericFileWhenTxtFilePathIsPassedAsArgument()
        {
            // Arrange
            string path = "C:\\Program Files\\dummy.txt";

            // Directory.Exists must be mocked
            Win32Program.DirectoryWrapper = GetMockedDirectoryWrapper();

            // Act
            Win32Program.ApplicationType appType = Win32Program.GetAppTypeFromPath(path);

            // Assert
            Assert.AreEqual(appType, Win32Program.ApplicationType.GenericFile);
        }
    }
}
