using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace BasicSettings
{
    /// <summary>
    /// This class is for loading .ini files containing settings for the application.
    /// 
    /// All key/value pairs found before a [Section] has been defined are automatically
    /// assigned to the [Default] section.
    /// 
    /// If constructed with the default constructor the settings will be assumed to be in a file
    /// with the executable called "settigns.ini".
    /// 
    /// The .ini file can contain any number of [Sections] and key/value pairs.  It is up to the 
    /// developer to access the required data in the host application.
    /// </summary>
    public class CSettings
    {
        private List<String> m_iniText;
        private Dictionary<String, StringDictionary> m_attributeList;
        private String m_fileName;
        private bool m_dirty;

        /// <summary>
        /// The Configuration property should probably be removed.  For the time being I'm leaving it
        /// in to facilitate debugging - it allows the developer to access all meaningful lines of 
        /// text from the .ini file (meaning all comments are stripped).
        /// </summary>
        public List<String> Configuration
        {
            get
            {
                return m_iniText;
            }
        }

        public Dictionary<String, StringDictionary> Attributes
        {
            get
            {
                return m_attributeList;
            }
        }

        public CSettings()
        {
            m_fileName = "settings.ini";
            m_iniText = new List<String>();
            m_attributeList = new Dictionary<String, StringDictionary>();
            m_dirty = false;
        }

        public CSettings(String fileName)
        {
            m_fileName = fileName;
            m_iniText = new List<String>();
            m_attributeList = new Dictionary<String, StringDictionary>();
            m_dirty = false;
        }

        public StringDictionary GetSection(String sectionName)
        {
            if (m_attributeList.ContainsKey(sectionName))
                return m_attributeList[sectionName];
            return null;
        }

        public bool LoadSettings()
        {
            bool success = false;
            try
            {
                using (StreamReader reader = new StreamReader(m_fileName))
                {
                    String line = "";
                    int index = 0;

                    String sectionName = "[Default]";
                    StringDictionary section = new StringDictionary();
                    m_attributeList.Add(sectionName, section);

                    while (!reader.EndOfStream)
                    {
                        line = reader.ReadLine();
                        if (line.Contains(';')) // ini file comment
                        {
                            index = line.IndexOf(';');
                            int lastSemi = line.LastIndexOf(';');
                            if(line.Contains("\""))
                            {
                                // okay, tricky tricky might be a ; inside of "quoted;need this here"
                                int quoteIndex = line.LastIndexOf('\"');
                                if (index < quoteIndex)
                                {
                                    String afterQuote = line.Substring(quoteIndex + 1);
                                    index = quoteIndex + 1 + afterQuote.IndexOf(';');
                                }
                            }
                            if(index < line.Length)
                                line = line.Remove(index);
                        }
                        if (line.Length < 1)
                            continue;

                        m_iniText.Add(line);

                        if (line.StartsWith("["))
                        {
                            // check to see if this [Section] exists.  If so, add new entries to it
                            if (m_attributeList.ContainsKey(line.ToString().Trim()))
                            {
                                continue;
                            }
                            // otherwise, create a new [Section]
                            sectionName = line;
                            section = new StringDictionary();
                            m_attributeList.Add(sectionName, section);
                            continue;
                        }

                        String[] parts = line.Split('=');
                        if (parts.Length < 2)
                        {
                            Console.WriteLine("{0} : Invalid key/value pair in ini file: {1}", DateTime.Now.ToString(), line);
                            continue;
                        }
                        String key = parts[0].Trim();
                        String val = line.Remove(0, parts[0].Length + 1).Trim();

                        // check for existing attributes.  If we're trying to add the same attribute again we remove the
                        // old and add the new - so later definitions override earlier ones.
                        if(section.ContainsKey(key))
                        {
                            section.Remove(key);
                            section.Add(key, val);
                        }
                        else
                            section.Add(key, val);
                    }
                }

                success = true;
            }
            catch (Exception ex)
            {
                Console.WriteLine("{0} : Exception : {1}", DateTime.Now.ToString(), ex.Message);
                success = false;
            }

            return success;
        }

        public bool SaveSettings()
        {
            if (m_dirty)
            {
                using (StreamWriter writer = new StreamWriter(m_fileName))
                {
                    foreach (String section in m_attributeList.Keys)
                    {
                        String sectionTitle = section;
                        try
                        {
                            writer.WriteLine(sectionTitle);
                        }
                        catch (Exception e)
                        {
                            Console.WriteLine("{0} : Exception : {1}", DateTime.Now.ToString(), e.Message);
                            return false;
                        }
                        foreach (String attribute in m_attributeList[section].Keys)
                        {
                            try
                            {
                                writer.WriteLine(attribute.ToUpper() + "=" + m_attributeList[section][attribute]);
                            }
                            catch (Exception e)
                            {
                                Console.WriteLine("{0} : Exception : {1}", DateTime.Now.ToString(), e.Message);
                                return false;
                            }
                        }
                    }
                }
                m_dirty = false;
            }
            return true;
        }

        public bool Set(String sectionName, String key, String value)
        {
            if (sectionName.Equals(""))
                sectionName = "[Default]";

            if(m_attributeList.ContainsKey(sectionName))
            {
                if (m_attributeList[sectionName].ContainsKey(key))
                {
                    m_attributeList[sectionName].Remove(key);
                    m_attributeList[sectionName].Add(key, value);
                    m_dirty = true;
                }
            }
            if (!m_dirty)
            {
                StringDictionary newSection = new StringDictionary();
                m_attributeList.Add(sectionName, newSection);
                newSection.Add(key, value);
                m_dirty = true;
            }
            return true;
        }
    }
}
