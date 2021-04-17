#pragma once

#include <AngelscriptPlugin/AngelscriptPluginDLL.h>

#include <Core/Scripting/AngelscriptHelper.h>
#include <Foundation/Containers/ArrayMap.h>

#ifdef BUILDSYSTEM_ENABLE_ANGELSCRIPT_SUPPORT

//---------------------------
// Compilation settings
//

// Set this flag to turn on/off metadata processing
//  0 = off
//  1 = on
#  ifndef AS_PROCESS_METADATA
#    define AS_PROCESS_METADATA 1
#  endif

class ezAngelscriptBuilder;

// This callback will be called for each #include directive encountered by the
// builder. The callback should call the AddSectionFromFile or AddSectionFromMemory
// to add the included section to the script. If the include cannot be resolved
// then the function should return a negative value to abort the compilation.
typedef int (*INCLUDECALLBACK_t)(const char* include, const char* from, ezAngelscriptBuilder* builder, void* userParam);

// This callback will be called for each #pragma directive encountered by the builder.
// The application can interpret the pragmaText and decide what do to based on that.
// If the callback returns a negative value the builder will report an error and abort the compilation.
typedef int (*PRAGMACALLBACK_t)(const ezString& pragmaText, ezAngelscriptBuilder& builder, void* userParam);

// Helper class for loading and pre-processing script files to
// support include directives and metadata declarations
class EZ_ANGELSCRIPTPLUGIN_DLL ezAngelscriptBuilder
{
public:
  ezAngelscriptBuilder();
  ~ezAngelscriptBuilder() = default;

  // Start a new module
  ezInt32 StartNewModule(asIScriptEngine* engine, const char* moduleName);

  // Load a script section from a file on disk
  // Returns  1 if the file was included
  //          0 if the file had already been included before
  //         <0 on error
  ezInt32 AddSectionFromFile(const char* filename);

  // Load a script section from memory
  // Returns  1 if the section was included
  //          0 if a section with the same name had already been included before
  //         <0 on error
  ezInt32 AddSectionFromMemory(const char* sectionName, const char* scriptCode, ezUInt32 scriptLength = 0, ezInt32 lineOffset = 0);

  // Build the added script sections
  ezInt32 BuildModule();

  // Returns the engine
  asIScriptEngine* GetEngine();

  // Returns the current module
  asIScriptModule* GetModule();

  // Register the callback for resolving include directive
  void SetIncludeCallback(INCLUDECALLBACK_t callback, void* userParam);

  // Register the callback for resolving pragma directive
  void SetPragmaCallback(PRAGMACALLBACK_t callback, void* userParam);

  // Add a pre-processor define for conditional compilation
  void DefineWord(const char* word);

  // Enumerate included script sections
  ezUInt32 GetSectionCount() const;
  ezString GetSectionName(ezUInt32 index) const;

#  if AS_PROCESS_METADATA == 1
  // Get metadata declared for classes, interfaces, and enums
  ezDynamicArray<ezString> GetMetadataForType(ezInt32 typeId);

  // Get metadata declared for functions
  ezDynamicArray<ezString> GetMetadataForFunc(asIScriptFunction* func);

  // Get metadata declared for global variables
  ezDynamicArray<ezString> GetMetadataForVar(ezInt32 varIndex);

  // Get metadata declared for class variables
  ezDynamicArray<ezString> GetMetadataForTypeProperty(ezInt32 typeId, ezInt32 varIndex);

  // Get metadata declared for class methods
  ezDynamicArray<ezString> GetMetadataForTypeMethod(ezInt32 typeId, asIScriptFunction* method);
#  endif

protected:
  void ClearAll();
  ezInt32 Build();
  ezInt32 ProcessScriptSection(const char* script, ezUInt32 length, const char* sectionname, ezInt32 lineOffset);
  ezInt32 LoadScriptSection(const char* filename);
  bool IncludeIfNotAlreadyIncluded(const char* filename);

  ezInt32 SkipStatement(ezInt32 pos);

  ezInt32 ExcludeCode(ezInt32 start);
  void OverwriteCode(ezInt32 start, ezInt32 len);

  asIScriptEngine* engine;
  asIScriptModule* module;
  ezString modifiedScript;

  INCLUDECALLBACK_t includeCallback;
  void* includeParam;

  PRAGMACALLBACK_t pragmaCallback;
  void* pragmaParam;

#  if AS_PROCESS_METADATA == 1
  ezInt32 ExtractMetadata(ezInt32 pos, ezDynamicArray<ezString>& outMetadata);
  ezInt32 ExtractDeclaration(ezInt32 pos, ezString& outName, ezStringBuilder& outDeclaration, ezInt32& outType);

  enum METADATATYPE
  {
    MDT_TYPE = 1,
    MDT_FUNC = 2,
    MDT_VAR = 3,
    MDT_VIRTPROP = 4,
    MDT_FUNC_OR_VAR = 5
  };

  // Temporary structure for storing metadata and declaration
  struct SMetadataDecl
  {
    SMetadataDecl() = default;
    SMetadataDecl(ezDynamicArray<ezString> m, ezString n, ezString d, ezInt32 t, ezString c, ezString ns)
      : metadata(m)
      , name(n)
      , declaration(d)
      , type(t)
      , parentClass(c)
      , nameSpace(ns)
    {
    }
    ezDynamicArray<ezString> metadata;
    ezString name;
    ezString declaration;
    ezInt32 type;
    ezString parentClass;
    ezString nameSpace;
  };
  ezDynamicArray<SMetadataDecl> foundDeclarations;
  ezString currentClass;
  ezStringBuilder currentNamespace;

  // Storage of metadata for global declarations
  ezArrayMap<ezInt32, ezDynamicArray<ezString>> typeMetadataMap;
  ezArrayMap<ezInt32, ezDynamicArray<ezString>> funcMetadataMap;
  ezArrayMap<ezInt32, ezDynamicArray<ezString>> varMetadataMap;

  // Storage of metadata for class member declarations
  struct SClassMetadata
  {
    SClassMetadata() = default;
    SClassMetadata(const ezString& aName)
      : className(aName)
    {
    }
    ezString className;
    ezArrayMap<ezInt32, ezDynamicArray<ezString>> funcMetadataMap;
    ezArrayMap<ezInt32, ezDynamicArray<ezString>> varMetadataMap;
  };
  ezArrayMap<ezInt32, SClassMetadata> classMetadataMap;

#  endif

#  if EZ_ENABLED(EZ_PLATFORM_WINDOWS)
  // On Windows the filenames are case insensitive so the comparisons to
  // avoid duplicate includes must also be case insensitive. 
  //

  // TODO: Strings by default are treated as UTF8 encoded. If the application choses to
  //       use a different encoding, the comparison algorithm should be adjusted as well

  ezSet<ezString, ezCompareString_NoCase> includedScripts;
#  else
  ezSet<ezString> includedScripts;
#  endif

  ezSet<ezString> definedWords;
};

#endif
