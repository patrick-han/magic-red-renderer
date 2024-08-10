#include <Rendering/Mesh/Mesh.h>

namespace MagicRed::Rendering
{
    void GPUMesh::cleanup(VmaAllocator allocator) {
        vertexBuffer.cleanup(allocator);
        indexBuffer.cleanup(allocator);
    }
}
