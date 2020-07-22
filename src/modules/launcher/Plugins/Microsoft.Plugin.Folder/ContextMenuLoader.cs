using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using System.Windows;
using Wox.Infrastructure.Logger;
using Wox.Infrastructure.Image;
using Wox.Plugin.SharedCommands;
using Wox.Plugin;
using System.Reflection;
using System.Windows.Input;
using Wox.Infrastructure;

namespace Microsoft.Plugin.Folder
{
    internal class ContextMenuLoader : IContextMenu
    {
        private readonly PluginInitContext _context;

        public ContextMenuLoader(PluginInitContext context)
        {
            _context = context;
        }

        public List<ContextMenuResult> LoadContextMenus(Result selectedResult)
        {
            var contextMenus = new List<ContextMenuResult>();
            if (selectedResult.ContextData is SearchResult record)
            {
                if (record.Type == ResultType.File)
                {
                    contextMenus.Add(CreateOpenContainingFolderResult(record));
                }

                var icoPath = (record.Type == ResultType.File) ? Main.FileImagePath : Main.FolderImagePath;
                contextMenus.Add(new ContextMenuResult
                {
                    PluginName = Assembly.GetExecutingAssembly().GetName().Name,
                    Title = _context.API.GetTranslation("Microsoft_plugin_folder_copy_path"),
                    Glyph = "\xE8C8",
                    FontFamily = "Segoe MDL2 Assets",
                    AcceleratorKey = Key.C,
                    AcceleratorModifiers = ModifierKeys.Control,
                    Action = (context) =>
                    {
                        try
                        {
                            Clipboard.SetText(record.FullPath);
                            return true;
                        }
                        catch (Exception e)
                        {
                            var message = "Fail to set text in clipboard";
                            LogException(message, e);
                            _context.API.ShowMsg(message);
                            return false;
                        }
                    }
                });

                contextMenus.Add(new ContextMenuResult
                {
                    PluginName = Assembly.GetExecutingAssembly().GetName().Name,
                    Title = _context.API.GetTranslation("Microsoft_plugin_folder_open_in_console"),
                    Glyph = "\xE756",
                    FontFamily = "Segoe MDL2 Assets",
                    AcceleratorKey = Key.C,
                    AcceleratorModifiers = ModifierKeys.Control | ModifierKeys.Shift,

                    Action = (context) =>
                    {
                        try
                        {
                            if (record.Type == ResultType.File)
                            {
                                Helper.OpenInConsole(Path.GetDirectoryName(record.FullPath));
                            }
                            else
                            {
                                Helper.OpenInConsole(record.FullPath);
                            }

                            return true;
                        }
                        catch (Exception e)
                        {
                            Log.Exception($"|Microsoft.Plugin.Folder.ContextMenuLoader.LoadContextMenus| Failed to open {record.FullPath} in console, {e.Message}", e);
                            return false;
                        }
                    }
                });
            }

            return contextMenus;
        }

        private ContextMenuResult CreateOpenContainingFolderResult(SearchResult record)
        {
            return new ContextMenuResult
            {
                PluginName = Assembly.GetExecutingAssembly().GetName().Name,
                Title = _context.API.GetTranslation("Microsoft_plugin_folder_open_containing_folder"),
                Glyph = "\xE838",
                FontFamily = "Segoe MDL2 Assets",
                AcceleratorKey = Key.E,
                AcceleratorModifiers = (ModifierKeys.Control | ModifierKeys.Shift),
                Action = _ =>
                {
                    try
                    {
                        Process.Start("explorer.exe", $" /select,\"{record.FullPath}\"");
                    }
                    catch (Exception e)
                    {
                        var message = $"Fail to open file at {record.FullPath}";
                        LogException(message, e);
                        _context.API.ShowMsg(message);
                        return false;
                    }

                    return true;
                }
            };
        }


        private Result CreateOpenWithEditorResult(SearchResult record)
        {
            string editorPath = "notepad.exe"; // TODO add the ability to create a custom editor

            var name = "Open With Editor: " + Path.GetFileNameWithoutExtension(editorPath);
            return new Result
            {
                Title = name,
                Action = _ =>
                {
                    try
                    {
                        Process.Start(editorPath, record.FullPath);
                        return true;
                    }
                    catch (Exception e)
                    {
                        var message = $"Fail to editor for file at {record.FullPath}";
                        LogException(message, e);
                        _context.API.ShowMsg(message);
                        return false;
                    }
                },
                IcoPath = editorPath
            };
        }

        public void LogException(string message, Exception e)
        {
            Log.Exception($"|Microsoft.Plugin.Folder.ContextMenu|{message}", e);
        }

        private bool CanRunAsDifferentUser(string path)
        {
            switch (Path.GetExtension(path))
            {
                case ".exe":
                case ".bat":
                    return true;

                default:
                    return false;

            }
        }
    }

    public class SearchResult
    {
        public string FullPath { get; set; }
        public ResultType Type { get; set; }
    }

    public enum ResultType
    {
        Volume,
        Folder,
        File
    }
}