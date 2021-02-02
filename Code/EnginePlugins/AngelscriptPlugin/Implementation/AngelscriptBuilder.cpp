#include <AngelscriptPlugin/AngelscriptBuilder.h>
#include <Core/Scripting/AngelscriptHelper.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/IO/OSFile.h>

// Helper functions
static ezString GetAbsolutePath(const ezString& path);

ezAngelscriptBuilder::ezAngelscriptBuilder()
{
  engine = nullptr;
  module = nullptr;

  includeCallback = nullptr;
  includeParam = nullptr;

  pragmaCallback = nullptr;
  pragmaParam = nullptr;
}

ezAngelscriptBuilder::~ezAngelscriptBuilder()
{
}

void ezAngelscriptBuilder::SetIncludeCallback(INCLUDECALLBACK_t callback, void* userParam)
{
  includeCallback = callback;
  includeParam = userParam;
}

void ezAngelscriptBuilder::SetPragmaCallback(PRAGMACALLBACK_t callback, void* userParam)
{
  pragmaCallback = callback;
  pragmaParam = userParam;
}

ezInt32 ezAngelscriptBuilder::StartNewModule(asIScriptEngine* inEngine, const char* moduleName)
{
  if (inEngine == 0)
    return -1;

  engine = inEngine;
  module = inEngine->GetModule(moduleName, asGM_ALWAYS_CREATE);
  if (module == 0)
    return -1;

  ClearAll();

  return 0;
}

asIScriptEngine* ezAngelscriptBuilder::GetEngine()
{
  return engine;
}

asIScriptModule* ezAngelscriptBuilder::GetModule()
{
  return module;
}

ezUInt32 ezAngelscriptBuilder::GetSectionCount() const
{
  return includedScripts.GetCount();
}

ezString ezAngelscriptBuilder::GetSectionName(ezUInt32 index) const
{
  if (index >= includedScripts.GetCount())
    return {};

  auto it = begin(includedScripts);
  while (index-- > 0)
  {
    //it++;
    it.Next();
  }
  return *it;
}

ezInt32 ezAngelscriptBuilder::AddSectionFromFile(const char* filename)
{
  // The file name stored in the set should be the fully resolved name because
  // it is possible to name the same file in multiple ways using relative paths.
  ezString fullpath = GetAbsolutePath(filename);

  if (IncludeIfNotAlreadyIncluded(fullpath.GetData()))
  {
    ezInt32 r = LoadScriptSection(fullpath.GetData());
    if (r < 0)
      return r;
    else
      return 1;
  }

  return 0;
}

ezInt32 ezAngelscriptBuilder::AddSectionFromMemory(const char* sectionName, const char* scriptCode, ezUInt32 scriptLength, ezInt32 lineOffset)
{
  if (IncludeIfNotAlreadyIncluded(sectionName))
  {
    ezInt32 r = ProcessScriptSection(scriptCode, scriptLength, sectionName, lineOffset);
    if (r < 0)
      return r;
    else
      return 1;
  }

  return 0;
}

ezInt32 ezAngelscriptBuilder::BuildModule()
{
  return Build();
}

void ezAngelscriptBuilder::DefineWord(const char* word)
{
  ezString sword = word;

  if (definedWords.Find(sword) == end(definedWords))
  {
    definedWords.Insert(sword);
  }
}

void ezAngelscriptBuilder::ClearAll()
{
  includedScripts.Clear();

#if AS_PROCESS_METADATA == 1
  currentClass = "";
  currentNamespace = "";

  foundDeclarations.Clear();
  typeMetadataMap.Clear();
  funcMetadataMap.Clear();
  varMetadataMap.Clear();
#endif
}

bool ezAngelscriptBuilder::IncludeIfNotAlreadyIncluded(const char* filename)
{
  ezString scriptFile = filename;
  if (includedScripts.Find(scriptFile) != end(includedScripts))
  {
    // Already included
    return false;
  }

  // Add the file to the set of included sections
  includedScripts.Insert(scriptFile);

  return true;
}

ezInt32 ezAngelscriptBuilder::LoadScriptSection(const char* filename)
{
  // Open the script file
  ezString scriptFile = filename;

  ezFileReader codeFile;
  if (!codeFile.Open(filename).Succeeded())
  {
    // Write a message to the engine's message callback
    ezStringBuilder msg("Failed to open script file '", GetAbsolutePath(scriptFile), "'");
    engine->WriteMessage(filename, 0, 0, asMSGTYPE_ERROR, msg.GetData());

    // TODO: Write the file where this one was included from
    return -1;
  }

  // Determine size of the file
  ezUInt32 codeFileSize = (ezUInt32)codeFile.GetFileSize();

  // Read the entire file
  ezString code;
  if (codeFileSize > 0)
  {
    code.ReadAll(codeFile);
  }
  codeFile.Close();

  if (code.GetCharacterCount() == 0 && codeFileSize > 0)
  {
    // Write a message to the engine's message callback
    ezStringBuilder msg("Failed to load script file '", GetAbsolutePath(scriptFile), "'");
    engine->WriteMessage(filename, 0, 0, asMSGTYPE_ERROR, msg.GetData());
    return -1;
  }

  // Process the script section even if it is zero length so that the name is registered
  return ProcessScriptSection(code.GetData(), code.GetCharacterCount(), filename, 0);
}

ezInt32 ezAngelscriptBuilder::ProcessScriptSection(const char* script, ezUInt32 length, const char* sectionname, ezInt32 lineOffset)
{
  ezDynamicArray<ezString> includes;

  // Perform a superficial parsing of the script first to store the metadata
  if (length)
    //modifiedScript.Assign(script, length);
    modifiedScript = script;
  else
    modifiedScript = script;

  // First perform the checks for #if directives to exclude code that shouldn't be compiled
  ezUInt32 pos = 0;
  ezInt32 nested = 0;
  while (pos < modifiedScript.GetCharacterCount())
  {
    asUINT len = 0;
    asETokenClass t = engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);
    if (t == asTC_UNKNOWN && modifiedScript[pos] == '#' && (pos + 1 < modifiedScript.GetCharacterCount()))
    {
      int start = pos++;

      // Is this an #if directive?
      t = engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);

      ezString token(modifiedScript.GetSubString(pos, len));

      pos += len;

      if (token == "if")
      {
        t = engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);
        if (t == asTC_WHITESPACE)
        {
          pos += len;
          t = engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);
        }

        if (t == asTC_IDENTIFIER)
        {
          ezString word(modifiedScript.GetSubString(pos, len));

          // Overwrite the #if directive with space characters to avoid compiler error
          pos += len;
          OverwriteCode(start, pos - start);

          // Has this identifier been defined by the application or not?
          if (definedWords.Find(word) ==end(definedWords))
          {
            // Exclude all the code until and including the #endif
            pos = ExcludeCode(pos);
          }
          else
          {
            nested++;
          }
        }
      }
      else if (token == "endif")
      {
        // Only remove the #endif if there was a matching #if
        if (nested > 0)
        {
          OverwriteCode(start, pos - start);
          nested--;
        }
      }
    }
    else
      pos += len;
  }

#if AS_PROCESS_METADATA == 1
  // Preallocate memory
  ezString name;
  ezStringBuilder declaration;
  ezDynamicArray<ezString> metadata;
  declaration.Reserve(100);
#endif

  // Then check for meta data and #include directives
  pos = 0;
  while (pos < modifiedScript.GetCharacterCount())
  {
    asUINT len = 0;
    asETokenClass t = engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);
    if (t == asTC_COMMENT || t == asTC_WHITESPACE)
    {
      pos += len;
      continue;
    }

#if AS_PROCESS_METADATA == 1
    // Check if class
    if (currentClass == "" && modifiedScript.GetSubString(pos, len) == "class")
    {
      // Get the identifier after "class"
      do
      {
        pos += len;
        if (pos >= modifiedScript.GetCharacterCount())
        {
          t = asTC_UNKNOWN;
          break;
        }
        t = engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);
      } while (t == asTC_COMMENT || t == asTC_WHITESPACE);

      if (t == asTC_IDENTIFIER)
      {
        currentClass = modifiedScript.GetSubString(pos, len);

        // Search until first { or ; is encountered
        while (pos < modifiedScript.GetCharacterCount())
        {
          engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);

          // If start of class section encountered stop
          if (modifiedScript[pos] == '{')
          {
            pos += len;
            break;
          }
          else if (modifiedScript[pos] == ';')
          {
            // The class declaration has ended and there are no children
            currentClass = "";
            pos += len;
            break;
          }

          // Check next symbol
          pos += len;
        }
      }

      continue;
    }

    // Check if end of class
    if (currentClass != "" && modifiedScript[pos] == '}')
    {
      currentClass = "";
      pos += len;
      continue;
    }

    // Check if namespace
    if (modifiedScript.GetSubString(pos, len) == "namespace")
    {
      // Get the identifier after "namespace"
      do
      {
        pos += len;
        t = engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);
      } while (t == asTC_COMMENT || t == asTC_WHITESPACE);

      if (currentNamespace != "")
        currentNamespace.Append("::");
      currentNamespace.Append(modifiedScript.GetSubString(pos, len));

      // Search until first { is encountered
      while (pos < modifiedScript.GetCharacterCount())
      {
        engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);

        // If start of namespace section encountered stop
        if (modifiedScript[pos] == '{')
        {
          pos += len;
          break;
        }

        // Check next symbol
        pos += len;
      }

      continue;
    }

    // Check if end of namespace
    if (currentNamespace != "" && modifiedScript[pos] == '}')
    {
      const char* found = currentNamespace.FindLastSubString("::");
      if (found != nullptr)
      {
        currentNamespace.ReplaceLast(found, "");
      }
      else
      {
        currentNamespace = "";
      }
      pos += len;
      continue;
    }

    // Is this the start of metadata?
    if (modifiedScript[pos] == '[')
    {
      // Get the metadata string
      pos = ExtractMetadata(pos, metadata);

      // Determine what this metadata is for
      int type;
      ExtractDeclaration(pos, name, declaration, type);

      // Store away the declaration in a map for lookup after the build has completed
      if (type > 0)
      {
        SMetadataDecl decl(metadata, name, declaration, type, currentClass, currentNamespace);
        foundDeclarations.PushBack(decl);
      }
    }
    else
#endif
      // Is this a preprocessor directive?
      if (modifiedScript[pos] == '#' && (pos + 1 < modifiedScript.GetCharacterCount()))
    {
      int start = pos++;

      t = engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);
      if (t == asTC_IDENTIFIER)
      {
        ezString token(modifiedScript.GetSubString(pos, len));
        if (token == "include")
        {
          pos += len;
          t = engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);
          if (t == asTC_WHITESPACE)
          {
            pos += len;
            t = engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);
          }

          if (t == asTC_VALUE && len > 2 && (modifiedScript[pos] == '"' || modifiedScript[pos] == '\''))
          {
            // Get the include file
            ezString includefile(modifiedScript.GetSubString(pos + 1, len - 2));
            pos += len;

            // Store it for later processing
            includes.PushBack(includefile);

            // Overwrite the include directive with space characters to avoid compiler error
            OverwriteCode(start, pos - start);
          }
        }
        else if (token == "pragma")
        {
          // Read until the end of the line
          pos += len;
          for (; pos < modifiedScript.GetCharacterCount() && modifiedScript[pos] != '\n'; pos++)
            ;

          // Call the pragma callback
          ezString pragmaText(modifiedScript.GetSubString(start + 7, pos - start - 7));
          int r = pragmaCallback ? pragmaCallback(pragmaText, *this, pragmaParam) : -1;
          if (r < 0)
          {
            // TODO: Report the correct line number
            engine->WriteMessage(sectionname, 0, 0, asMSGTYPE_ERROR, "Invalid #pragma directive");
            return r;
          }

          // Overwrite the pragma directive with space characters to avoid compiler error
          OverwriteCode(start, pos - start);
        }
      }
    }
    // Don't search for metadata/includes within statement blocks or between tokens in statements
    else
    {
      pos = SkipStatement(pos);
    }
  }

  // Build the actual script
  engine->SetEngineProperty(asEP_COPY_SCRIPT_SECTIONS, true);
  module->AddScriptSection(sectionname, modifiedScript.GetData(), modifiedScript.GetCharacterCount(), lineOffset);

  if (includes.GetCount() > 0)
  {
    // If the callback has been set, then call it for each included file
    if (includeCallback)
    {
      for (ezInt32 n = 0; n < (ezInt32)includes.GetCount(); n++)
      {
        ezInt32 r = includeCallback(includes[n].GetData(), sectionname, this, includeParam);
        if (r < 0)
          return r;
      }
    }
    else
    {
      // By default we try to load the included file from the relative directory of the current file

      // Determine the path of the current script so that we can resolve relative paths for includes

      ezStringBuilder path(sectionname);
      path.ReplaceAll("\\","/");
      const char pathSeparator[2] = {'/', '\0'};
      auto slash = path.FindLastSubString(pathSeparator);
      if (slash != nullptr)
        path.ReplaceLast(slash+1, "");
      else
        path = "";

      // Load the included scripts
      for (ezInt32 n = 0; n < (ezInt32)includes.GetCount(); n++)
      {
        // If the include is a relative path, then prepend the path of the originating script
        if (!includes[n].StartsWith(pathSeparator) && includes[n].FindSubString(":") == nullptr)
        {
          includes[n] = ezStringBuilder(path, includes[n]);
        }

        // Include the script section
        ezInt32 r = AddSectionFromFile(includes[n].GetData());
        if (r < 0)
          return r;
      }
    }
  }

  return 0;
}

ezInt32 ezAngelscriptBuilder::Build()
{
  ezInt32 r = module->Build();
  if (r < 0)
    return r;

#if AS_PROCESS_METADATA == 1
  // After the script has been built, the metadata strings should be
  // stored for later lookup by function id, type id, and variable index
  for (ezInt32 n = 0; n < (ezInt32)foundDeclarations.GetCount(); n++)
  {
    SMetadataDecl* decl = &foundDeclarations[n];
    module->SetDefaultNamespace(decl->nameSpace.GetData());
    if (decl->type == MDT_TYPE)
    {
      // Find the type id
      ezInt32 typeId = module->GetTypeIdByDecl(decl->declaration.GetData());
      EZ_ASSERT_ALWAYS(typeId >= 0, "TypeId '{0}' was not found.", decl->declaration);
      if (typeId >= 0)
        typeMetadataMap.Insert(typeId, decl->metadata);
    }
    else if (decl->type == MDT_FUNC)
    {
      if (decl->parentClass == "")
      {
        // Find the function id
        asIScriptFunction* func = module->GetFunctionByDecl(decl->declaration.GetData());
        EZ_ASSERT_ALWAYS(func, "Function with declaration '{0}' was not found.", decl->declaration);
        if (func)
          funcMetadataMap.Insert(func->GetId(), decl->metadata);
      }
      else
      {
        // Find the method id
        ezInt32 typeId = module->GetTypeIdByDecl(decl->parentClass.GetData());
        EZ_ASSERT_ALWAYS(typeId >= 0, "TypeId '{0}' was not found.", decl->declaration);
        ezUInt32 it = classMetadataMap.Find(typeId);
        if (it == ezInvalidIndex)
        {
          classMetadataMap.Insert(typeId, SClassMetadata(decl->parentClass));
          it = classMetadataMap.Find(typeId);
        }

        asITypeInfo* type = engine->GetTypeInfoById(typeId);
        asIScriptFunction* func = type->GetMethodByDecl(decl->declaration.GetData());
        EZ_ASSERT_ALWAYS(func, "Function with declaration '{0}' was not found.", decl->declaration);
        if (func)
          classMetadataMap.GetValue(it).funcMetadataMap.Insert(func->GetId(), decl->metadata);
      }
    }
    else if (decl->type == MDT_VIRTPROP)
    {
      if (decl->parentClass == "")
      {
        // Find the global virtual property accessors
        asIScriptFunction* func = module->GetFunctionByName(ezStringBuilder("get_", decl->declaration).GetData());
        if (func)
          funcMetadataMap.Insert(func->GetId(), decl->metadata);
        func = module->GetFunctionByName(ezStringBuilder("set_", decl->declaration).GetData());
        if (func)
          funcMetadataMap.Insert(func->GetId(), decl->metadata);
      }
      else
      {
        // Find the method virtual property accessors
        ezInt32 typeId = module->GetTypeIdByDecl(decl->parentClass.GetData());
        EZ_ASSERT_ALWAYS(typeId >= 0, "TypeId '{0}' was not found.", decl->parentClass);
        ezUInt32 it = classMetadataMap.Find(typeId);
        if (it == ezInvalidIndex)
        {
          classMetadataMap.Insert(typeId, SClassMetadata(decl->parentClass));
          it = classMetadataMap.Find(typeId);
        }

        asITypeInfo* type = engine->GetTypeInfoById(typeId);
        asIScriptFunction* func = type->GetMethodByName(ezStringBuilder("get_", decl->declaration).GetData());
        if (func)
          classMetadataMap.GetValue(it).funcMetadataMap.Insert(func->GetId(), decl->metadata);
        func = type->GetMethodByName(ezStringBuilder("set_", decl->declaration).GetData());
        if (func)
          classMetadataMap.GetValue(it).funcMetadataMap.Insert(func->GetId(), decl->metadata);
      }
    }
    else if (decl->type == MDT_VAR)
    {
      if (decl->parentClass == "")
      {
        // Find the global variable index
        ezInt32 varIdx = module->GetGlobalVarIndexByName(decl->declaration.GetData());
        EZ_ASSERT_ALWAYS(varIdx >= 0, "Variable '{0}' was not found.", decl->declaration);
        if (varIdx >= 0)
          varMetadataMap.Insert(varIdx, decl->metadata);
      }
      else
      {
        ezInt32 typeId = module->GetTypeIdByDecl(decl->parentClass.GetData());
        EZ_ASSERT_ALWAYS(typeId >= 0, "TypeId '{0}' was not found.", decl->parentClass);

        // Add the classes if needed
        ezUInt32 it = classMetadataMap.Find(typeId);
        if (it == ezInvalidIndex)
        {
          classMetadataMap.Insert(typeId, SClassMetadata(decl->parentClass));
          it = classMetadataMap.Find(typeId);
        }

        // Add the variable to class
        asITypeInfo* objectType = engine->GetTypeInfoById(typeId);
        ezInt32 idx = -1;

        // Search through all properties to get proper declaration
        for (asUINT i = 0; i < (asUINT)objectType->GetPropertyCount(); ++i)
        {
          const char* name;
          objectType->GetProperty(i, &name);
          if (decl->declaration == name)
          {
            idx = i;
            break;
          }
        }

        // If found, add it
        EZ_ASSERT_ALWAYS(idx >= 0, "Property not found.");
        if (idx >= 0)
          classMetadataMap.GetValue(it).varMetadataMap.Insert(idx, decl->metadata);
      }
    }
    else if (decl->type == MDT_FUNC_OR_VAR)
    {
      if (decl->parentClass == "")
      {
        // Find the global variable index
        ezInt32 varIdx = module->GetGlobalVarIndexByName(decl->name.GetData());
        if (varIdx >= 0)
          varMetadataMap.Insert(varIdx, decl->metadata);
        else
        {
          asIScriptFunction* func = module->GetFunctionByDecl(decl->declaration.GetData());
          EZ_ASSERT_ALWAYS(func, "Function with declaration '{0}' was not found.", decl->declaration);
          if (func)
            funcMetadataMap.Insert(func->GetId(), decl->metadata);
        }
      }
      else
      {
        int typeId = module->GetTypeIdByDecl(decl->parentClass.GetData());
        EZ_ASSERT_ALWAYS(typeId >= 0, "TypeId '{0}' was not found.", decl->parentClass);

        // Add the classes if needed
        ezUInt32 it = classMetadataMap.Find(typeId);
        if (it == ezInvalidIndex)
        {
          classMetadataMap.Insert(typeId, SClassMetadata(decl->parentClass));
          it = classMetadataMap.Find(typeId);
        }

        // Add the variable to class
        asITypeInfo* objectType = engine->GetTypeInfoById(typeId);
        ezInt32 idx = -1;

        // Search through all properties to get proper declaration
        for (asUINT i = 0; i < (asUINT)objectType->GetPropertyCount(); ++i)
        {
          const char* name;
          objectType->GetProperty(i, &name);
          if (decl->name == name)
          {
            idx = i;
            break;
          }
        }

        // If found, add it
        if (idx >= 0)
          classMetadataMap.GetValue(it).varMetadataMap.Insert(idx, decl->metadata);
        else
        {
          // Look for the matching method instead
          asITypeInfo* type = engine->GetTypeInfoById(typeId);
          asIScriptFunction* func = type->GetMethodByDecl(decl->declaration.GetData());
          EZ_ASSERT_ALWAYS(func, "Function with declaration '{0}' was not found.", decl->declaration);
          if (func)
            classMetadataMap.GetValue(it).funcMetadataMap.Insert(func->GetId(), decl->metadata);
        }
      }
    }
  }
  module->SetDefaultNamespace("");
#endif

  return 0;
}

ezInt32 ezAngelscriptBuilder::SkipStatement(ezInt32 pos)
{
  asUINT len = 0;

  // Skip until ; or { whichever comes first
  while (pos < (ezInt32)modifiedScript.GetCharacterCount() && modifiedScript[pos] != ';' && modifiedScript[pos] != '{')
  {
    engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);
    pos += len;
  }

  // Skip entire statement block
  if (pos < (ezInt32)modifiedScript.GetCharacterCount() && modifiedScript[pos] == '{')
  {
    pos += 1;

    // Find the end of the statement block
    ezInt32 level = 1;
    while (level > 0 && pos < (ezInt32)modifiedScript.GetCharacterCount())
    {
      asETokenClass t = engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);
      if (t == asTC_KEYWORD)
      {
        if (modifiedScript[pos] == '{')
          level++;
        else if (modifiedScript[pos] == '}')
          level--;
      }

      pos += len;
    }
  }
  else
    pos += 1;

  return pos;
}

ezInt32 ezAngelscriptBuilder::ExcludeCode(ezInt32 pos)
{
    asUINT len = 0;
  int nested = 0;
  while (pos < (ezInt32)modifiedScript.GetCharacterCount())
  {
    engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);
    if (modifiedScript[pos] == '#')
    {

      // nasty, I know
      char* modifiedScriptMut = const_cast<char*>(modifiedScript.GetData());
      modifiedScriptMut[pos] = ' ';
      //modifiedScript[pos] = ' ';
      pos++;

      // Is it an #if or #endif directive?
      engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);
      ezString token(modifiedScript.GetSubString(pos, len));
      OverwriteCode(pos, len);

      if (token == "if")
      {
        nested++;
      }
      else if (token == "endif")
      {
        if (nested-- == 0)
        {
          pos += len;
          break;
        }
      }
    }
    else if (modifiedScript[pos] != '\n')
    {
      OverwriteCode(pos, len);
    }
    pos += len;
  }

  return pos;
}

void ezAngelscriptBuilder::OverwriteCode(ezInt32 start, ezInt32 len)
{
  // nasty, I know... ezString doesn't support this though
  char* modifiedScriptMut = const_cast<char*>(modifiedScript.GetData());
  char* code = &modifiedScriptMut[start];
  //char* code = &modifiedScript[start];
  for (ezInt32 n = 0; n < len; n++)
  {
    if (*code != '\n')
      *code = ' ';
    code++;
  }
}

#if AS_PROCESS_METADATA == 1
ezInt32 ezAngelscriptBuilder::ExtractMetadata(ezInt32 pos, ezDynamicArray<ezString>& metadata)
{
  metadata.Clear();

  // Extract all metadata. They can be separated by whitespace and comments
  for (;;)
  {
    ezStringBuilder metadataString = "";

    // Overwrite the metadata with space characters to allow compilation
    char* modifiedScriptMut = const_cast<char*>(modifiedScript.GetData());
    modifiedScriptMut[pos] = ' ';
    //modifiedScript[pos] = ' ';

    // Skip opening brackets
    pos += 1;

    ezInt32 level = 1;
    asUINT len = 0;
    while (level > 0 && pos < (ezInt32)modifiedScript.GetCharacterCount())
    {
      asETokenClass t = engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);
      if (t == asTC_KEYWORD)
      {
        if (modifiedScript[pos] == '[')
          level++;
        else if (modifiedScript[pos] == ']')
          level--;
      }

      // Copy the metadata to our buffer
      if (level > 0)
        metadataString.Append(modifiedScript.GetSubString(pos, len));

      // Overwrite the metadata with space characters to allow compilation
      if (t != asTC_WHITESPACE)
        OverwriteCode(pos, len);

      pos += len;
    }

    metadata.PushBack(metadataString);

    // Check for more metadata. Possibly separated by comments
    asETokenClass t = engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);
    while (t == asTC_COMMENT || t == asTC_WHITESPACE)
    {
      pos += len;
      t = engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);
    }

    if (modifiedScript[pos] != '[')
      break;
  }

  return pos;
}

ezInt32 ezAngelscriptBuilder::ExtractDeclaration(ezInt32 pos, ezString& name, ezStringBuilder& declaration, ezInt32& type)
{
  declaration = "";
  type = 0;

  ezInt32 start = pos;

  ezString token;
  asUINT len = 0;
  asETokenClass t = asTC_WHITESPACE;

  // Skip white spaces, comments, and leading decorators
  do
  {
    pos += len;
    t = engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);
    token = modifiedScript.GetSubString(pos, len);
  } while (t == asTC_WHITESPACE || t == asTC_COMMENT ||
           token == "private" || token == "protected" ||
           token == "shared" || token == "external" ||
           token == "final" || token == "abstract");

  // We're expecting, either a class, interface, function, or variable declaration
  if (t == asTC_KEYWORD || t == asTC_IDENTIFIER)
  {
    token = modifiedScript.GetSubString(pos, len);
    if (token == "interface" || token == "class" || token == "enum")
    {
      // Skip white spaces and comments
      do
      {
        pos += len;
        t = engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);
      } while (t == asTC_WHITESPACE || t == asTC_COMMENT);

      if (t == asTC_IDENTIFIER)
      {
        type = MDT_TYPE;
        declaration = modifiedScript.GetSubString(pos, len);
        pos += len;
        return pos;
      }
    }
    else
    {
      // For function declarations, store everything up to the start of the
      // statement block, except for succeeding decorators (final, override, etc)

      // For variable declaration store just the name as there can only be one

      // We'll only know if the declaration is a variable or function declaration
      // when we see the statement block, or absense of a statement block.
      bool hasParenthesis = false;
      int nestedParenthesis = 0;
      declaration = modifiedScript.GetSubString(pos, len);
      pos += len;
      for (; pos < (int)modifiedScript.GetCharacterCount();)
      {
        t = engine->ParseToken(&modifiedScript[pos], modifiedScript.GetCharacterCount() - pos, &len);
        token = modifiedScript.GetSubString(pos, len);
        if (t == asTC_KEYWORD)
        {
          if (token == "{" && nestedParenthesis == 0)
          {
            if (hasParenthesis)
            {
              // We've found the end of a function signature
              type = MDT_FUNC;
            }
            else
            {
              // We've found a virtual property. Just keep the name
              declaration = name;
              type = MDT_VIRTPROP;
            }
            return pos;
          }
          if ((token == "=" && !hasParenthesis) || token == ";")
          {
            if (hasParenthesis)
            {
              // The declaration is ambigous. It can be a variable with initialization, or a function prototype
              type = MDT_FUNC_OR_VAR;
            }
            else
            {
              // Substitute the declaration with just the name
              declaration = name;
              type = MDT_VAR;
            }
            return pos;
          }
          else if (token == "(")
          {
            nestedParenthesis++;

            // This is the first parenthesis we encounter. If the parenthesis isn't followed
            // by a statement block, then this is a variable declaration, in which case we
            // should only store the type and name of the variable, not the initialization parameters.
            hasParenthesis = true;
          }
          else if (token == ")")
          {
            nestedParenthesis--;
          }
        }
        else if (t == asTC_IDENTIFIER)
        {
          name = token;
        }

        // Skip trailing decorators
        if (!hasParenthesis || nestedParenthesis > 0 || t != asTC_IDENTIFIER || (token != "final" && token != "override"))
          declaration.Append(token.GetData());

        pos += len;
      }
    }
  }

  return start;
}

ezDynamicArray<ezString> ezAngelscriptBuilder::GetMetadataForType(ezInt32 typeId)
{
  ezUInt32 index = typeMetadataMap.Find(typeId);
  if (index != ezInvalidIndex)
    return typeMetadataMap.GetValue(index);

  return ezDynamicArray<ezString>();
}

ezDynamicArray<ezString> ezAngelscriptBuilder::GetMetadataForFunc(asIScriptFunction* func)
{
  if (func)
  {
    ezUInt32 index = funcMetadataMap.Find(func->GetId());
    if (index != ezInvalidIndex)
      return funcMetadataMap.GetValue(index);
  }

  return ezDynamicArray<ezString>();
}

ezDynamicArray<ezString> ezAngelscriptBuilder::GetMetadataForVar(ezInt32 varIndex)
{
  ezUInt32 index = varMetadataMap.Find(varIndex);
  if (index != ezInvalidIndex)
    return varMetadataMap.GetValue(index);

  return ezDynamicArray<ezString>();
}

ezDynamicArray<ezString> ezAngelscriptBuilder::GetMetadataForTypeProperty(ezInt32 typeId, ezInt32 varIndex)
{
  ezUInt32 typeIndex = classMetadataMap.Find(typeId);
  if (typeIndex == ezInvalidIndex)
    return ezDynamicArray<ezString>();

  ezUInt32 propIndex = classMetadataMap.GetValue(typeIndex).varMetadataMap.Find(varIndex);
  if (propIndex == ezInvalidIndex)
    return ezDynamicArray<ezString>();

  return classMetadataMap.GetValue(typeIndex).varMetadataMap.GetValue(propIndex);
}

ezDynamicArray<ezString> ezAngelscriptBuilder::GetMetadataForTypeMethod(ezInt32 typeId, asIScriptFunction* method)
{
  if (method)
  {
    ezUInt32 typeIt = classMetadataMap.Find(typeId);
    if (typeIt == ezInvalidIndex)
      return ezDynamicArray<ezString>();

    ezUInt32 methodIt = classMetadataMap.GetValue(typeIt).funcMetadataMap.Find(method->GetId());
    if (methodIt == ezInvalidIndex)
      return ezDynamicArray<ezString>();

    return classMetadataMap.GetValue(typeIt).funcMetadataMap.GetValue(methodIt);
  }

  return ezDynamicArray<ezString>();
}
#endif

ezString GetAbsolutePath(const ezString& file)
{
  ezStringBuilder path;

  if (!ezPathUtils::IsAbsolutePath(file))
  {
    if (ezFileSystem::ResolveSpecialDirectory(">project", path).Succeeded())
    {
      path.Append("/", file.GetData());
      if (ezFileSystem::ExistsFile(path))
      {
        path.MakeCleanPath();
        return path;
      }
    }

    path = ezOSFile::GetApplicationDirectory();
    path.Append(file.GetData());

    // todo: verify that file exists
  }
  else
  {
    path = file;
  }

  path.MakeCleanPath();
  return path;
}
