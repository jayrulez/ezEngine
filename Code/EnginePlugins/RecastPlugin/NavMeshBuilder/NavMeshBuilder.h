#pragma once

#include <Core/Utils/WorldGeoExtractionUtil.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Types/UniquePtr.h>
#include <RecastPlugin/RecastPluginDLL.h>

class ezRcBuildContext;
struct rcPolyMesh;
struct rcPolyMeshDetail;
class ezWorld;
class dtNavMesh;
struct ezRecastNavMeshResourceDescriptor;
class ezProgress;
class ezStreamWriter;
class ezStreamReader;

struct EZ_RECASTPLUGIN_DLL ezRecastConfig
{
  float m_fAgentHeight = 1.5f;
  float m_fAgentRadius = 0.3f;
  float m_fAgentClimbHeight = 0.4f;
  ezAngle m_WalkableSlope = ezAngle::Degree(45);
  float m_fCellSize = 0.2f;
  float m_fCellHeight = 0.2f;
  float m_fMaxEdgeLength = 4.0f;
  float m_fMaxSimplificationError = 1.3f;
  float m_fMinRegionSize = 3.0f;
  float m_fRegionMergeSize = 20.0f;
  float m_fDetailMeshSampleDistanceFactor = 1.0f;
  float m_fDetailMeshSampleErrorFactor = 1.0f;

  ezResult Serialize(ezStreamWriter& stream) const;
  ezResult Deserialize(ezStreamReader& stream);
};

EZ_DECLARE_REFLECTABLE_TYPE(EZ_RECASTPLUGIN_DLL, ezRecastConfig);



class EZ_RECASTPLUGIN_DLL ezRecastNavMeshBuilder
{
public:
  ezRecastNavMeshBuilder();
  ~ezRecastNavMeshBuilder();

  static ezResult ExtractWorldGeometry(const ezWorld& world, ezWorldGeoExtractionUtil::Geometry& out_worldGeo);

  ezResult Build(const ezRecastConfig& config, const ezWorldGeoExtractionUtil::Geometry& worldGeo, ezRecastNavMeshResourceDescriptor& out_NavMeshDesc,
    ezProgress& progress);

private:
  static void FillOutConfig(struct rcConfig& cfg, const ezRecastConfig& config, const ezBoundingBox& bbox);

  void Clear();
  void ReserveMemory(const ezWorldGeoExtractionUtil::Geometry& desc);
  void GenerateTriangleMeshFromDescription(const ezWorldGeoExtractionUtil::Geometry& desc);
  void ComputeBoundingBox();
  ezResult BuildRecastPolyMesh(const ezRecastConfig& config, rcPolyMesh& out_PolyMesh, ezProgress& progress);
  static ezResult BuildDetourNavMeshData(const ezRecastConfig& config, const rcPolyMesh& polyMesh, ezDataBuffer& NavmeshData);

  struct Triangle
  {
    Triangle() {}
    Triangle(ezInt32 a, ezInt32 b, ezInt32 c)
    {
      m_VertexIdx[0] = a;
      m_VertexIdx[1] = b;
      m_VertexIdx[2] = c;
    }

    ezInt32 m_VertexIdx[3];
  };

  ezBoundingBox m_BoundingBox;
  ezDynamicArray<ezVec3> m_Vertices;
  ezDynamicArray<Triangle> m_Triangles;
  ezDynamicArray<ezUInt8> m_TriangleAreaIDs;
  ezRcBuildContext* m_pRecastContext = nullptr;
};
