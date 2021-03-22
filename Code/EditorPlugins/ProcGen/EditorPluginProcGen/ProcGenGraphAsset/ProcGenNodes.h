#pragma once

#include <Foundation/Types/TagSet.h>
#include <ProcGenPlugin/Resources/ProcGenGraphSharedData.h>
#include <ProcGenPlugin/VM/ExpressionAST.h>
#include <RendererCore/Pipeline/RenderPipelineNode.h>

class ezProcGenNodeBase : public ezReflectedClass
{
  EZ_ADD_DYNAMIC_REFLECTION(ezProcGenNodeBase, ezReflectedClass);

public:
  struct GenerateASTContext
  {
    ezProcGenInternal::GraphSharedData m_SharedData;
    ezHybridArray<ezUInt8, 4> m_VolumeTagSetIndices;
  };

  virtual ezExpressionAST::Node* GenerateExpressionASTNode(ezTempHashedString sOutputName, ezArrayPtr<ezExpressionAST::Node*> inputs, ezExpressionAST& out_Ast, GenerateASTContext& context) = 0;
};

//////////////////////////////////////////////////////////////////////////

class ezProcGenOutput : public ezProcGenNodeBase
{
  EZ_ADD_DYNAMIC_REFLECTION(ezProcGenOutput, ezProcGenNodeBase);

public:
  bool m_bActive = true;

  void Save(ezStreamWriter& stream);

  ezString m_sName;

  ezHybridArray<ezUInt8, 4> m_VolumeTagSetIndices;
};

//////////////////////////////////////////////////////////////////////////

class ezProcGen_PlacementOutput : public ezProcGenOutput
{
  EZ_ADD_DYNAMIC_REFLECTION(ezProcGen_PlacementOutput, ezProcGenOutput);

public:
  virtual ezExpressionAST::Node* GenerateExpressionASTNode(ezTempHashedString sOutputName, ezArrayPtr<ezExpressionAST::Node*> inputs, ezExpressionAST& out_Ast, GenerateASTContext& context) override;

  void Save(ezStreamWriter& stream);

  ezHybridArray<ezString, 4> m_ObjectsToPlace;

  float m_fFootprint = 1.0f;

  ezVec3 m_vMinOffset = ezVec3(0);
  ezVec3 m_vMaxOffset = ezVec3(0);

  float m_fAlignToNormal = 1.0f;

  ezVec3 m_vMinScale = ezVec3(1);
  ezVec3 m_vMaxScale = ezVec3(1);

  float m_fCullDistance = 30.0f;

  ezUInt32 m_uiCollisionLayer = 0;

  ezString m_sSurface;

  ezString m_sColorGradient;

  ezRenderPipelineNodeInputPin m_DensityPin;
  ezRenderPipelineNodeInputPin m_ScalePin;
  ezRenderPipelineNodeInputPin m_ColorIndexPin;
  ezRenderPipelineNodeInputPin m_ObjectIndexPin;
};

//////////////////////////////////////////////////////////////////////////

class ezProcGen_VertexColorOutput : public ezProcGenOutput
{
  EZ_ADD_DYNAMIC_REFLECTION(ezProcGen_VertexColorOutput, ezProcGenOutput);

public:
  virtual ezExpressionAST::Node* GenerateExpressionASTNode(ezTempHashedString sOutputName, ezArrayPtr<ezExpressionAST::Node*> inputs, ezExpressionAST& out_Ast, GenerateASTContext& context) override;

  void Save(ezStreamWriter& stream);

  ezRenderPipelineNodeInputPin m_RPin;
  ezRenderPipelineNodeInputPin m_GPin;
  ezRenderPipelineNodeInputPin m_BPin;
  ezRenderPipelineNodeInputPin m_APin;
};

//////////////////////////////////////////////////////////////////////////

class ezProcGen_Random : public ezProcGenNodeBase
{
  EZ_ADD_DYNAMIC_REFLECTION(ezProcGen_Random, ezProcGenNodeBase);

public:
  virtual ezExpressionAST::Node* GenerateExpressionASTNode(ezTempHashedString sOutputName, ezArrayPtr<ezExpressionAST::Node*> inputs, ezExpressionAST& out_Ast, GenerateASTContext& context) override;

  ezInt32 m_iSeed = -1;

  float m_fOutputMin = 0.0f;
  float m_fOutputMax = 1.0f;

  ezRenderPipelineNodeOutputPin m_OutputValuePin;

private:
  void OnObjectCreated(const ezAbstractObjectNode& node);

  ezUInt32 m_uiAutoSeed;
};

//////////////////////////////////////////////////////////////////////////

class ezProcGen_PerlinNoise : public ezProcGenNodeBase
{
  EZ_ADD_DYNAMIC_REFLECTION(ezProcGen_PerlinNoise, ezProcGenNodeBase);

public:
  virtual ezExpressionAST::Node* GenerateExpressionASTNode(ezTempHashedString sOutputName, ezArrayPtr<ezExpressionAST::Node*> inputs, ezExpressionAST& out_Ast, GenerateASTContext& context) override;

  ezVec3 m_Scale = ezVec3(10);
  ezVec3 m_Offset = ezVec3::ZeroVector();
  ezUInt32 m_uiNumOctaves = 3;

  float m_fOutputMin = 0.0f;
  float m_fOutputMax = 1.0f;

  ezRenderPipelineNodeOutputPin m_OutputValuePin;
};

//////////////////////////////////////////////////////////////////////////

class ezProcGen_Blend : public ezProcGenNodeBase
{
  EZ_ADD_DYNAMIC_REFLECTION(ezProcGen_Blend, ezProcGenNodeBase);

public:
  virtual ezExpressionAST::Node* GenerateExpressionASTNode(ezTempHashedString sOutputName, ezArrayPtr<ezExpressionAST::Node*> inputs, ezExpressionAST& out_Ast, GenerateASTContext& context) override;

  ezEnum<ezProcGenBlendMode> m_BlendMode;
  float m_fInputValueA = 1.0f;
  float m_fInputValueB = 1.0f;
  bool m_bClampOutput = false;

  ezRenderPipelineNodeInputPin m_InputValueAPin;
  ezRenderPipelineNodeInputPin m_InputValueBPin;
  ezRenderPipelineNodeOutputPin m_OutputValuePin;
};

//////////////////////////////////////////////////////////////////////////

class ezProcGen_Height : public ezProcGenNodeBase
{
  EZ_ADD_DYNAMIC_REFLECTION(ezProcGen_Height, ezProcGenNodeBase);

public:
  virtual ezExpressionAST::Node* GenerateExpressionASTNode(ezTempHashedString sOutputName, ezArrayPtr<ezExpressionAST::Node*> inputs, ezExpressionAST& out_Ast, GenerateASTContext& context) override;

  float m_fMinHeight = 0.0f;
  float m_fMaxHeight = 1000.0f;
  float m_fLowerFade = 0.2f;
  float m_fUpperFade = 0.2f;

  ezRenderPipelineNodeOutputPin m_OutputValuePin;
};

//////////////////////////////////////////////////////////////////////////

class ezProcGen_Slope : public ezProcGenNodeBase
{
  EZ_ADD_DYNAMIC_REFLECTION(ezProcGen_Slope, ezProcGenNodeBase);

public:
  virtual ezExpressionAST::Node* GenerateExpressionASTNode(ezTempHashedString sOutputName, ezArrayPtr<ezExpressionAST::Node*> inputs, ezExpressionAST& out_Ast, GenerateASTContext& context) override;

  ezAngle m_MinSlope = ezAngle::Degree(0.0f);
  ezAngle m_MaxSlope = ezAngle::Degree(30.0f);
  float m_fLowerFade = 0.0f;
  float m_fUpperFade = 0.2f;

  ezRenderPipelineNodeOutputPin m_OutputValuePin;
};

//////////////////////////////////////////////////////////////////////////

class ezProcGen_MeshVertexColor : public ezProcGenNodeBase
{
  EZ_ADD_DYNAMIC_REFLECTION(ezProcGen_MeshVertexColor, ezProcGenNodeBase);

public:
  virtual ezExpressionAST::Node* GenerateExpressionASTNode(ezTempHashedString sOutputName, ezArrayPtr<ezExpressionAST::Node*> inputs, ezExpressionAST& out_Ast, GenerateASTContext& context) override;

  ezRenderPipelineNodeOutputPin m_RPin;
  ezRenderPipelineNodeOutputPin m_GPin;
  ezRenderPipelineNodeOutputPin m_BPin;
  ezRenderPipelineNodeOutputPin m_APin;
};

//////////////////////////////////////////////////////////////////////////

class ezProcGen_ApplyVolumes : public ezProcGenNodeBase
{
  EZ_ADD_DYNAMIC_REFLECTION(ezProcGen_ApplyVolumes, ezProcGenNodeBase);

public:
  virtual ezExpressionAST::Node* GenerateExpressionASTNode(ezTempHashedString sOutputName, ezArrayPtr<ezExpressionAST::Node*> inputs, ezExpressionAST& out_Ast, GenerateASTContext& context) override;

  float m_fInputValue = 0.0f;

  ezTagSet m_IncludeTags;

  ezRenderPipelineNodeInputPin m_InputValuePin;
  ezRenderPipelineNodeOutputPin m_OutputValuePin;
};
