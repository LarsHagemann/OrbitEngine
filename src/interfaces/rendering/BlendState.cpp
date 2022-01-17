#include "interfaces/rendering/BlendState.hpp"

namespace orbit
{

    bool IBlendState::LoadImpl(std::ifstream* stream)
    {
        stream->read((char*)m_blendFactor, 4 * sizeof(float));
        stream->read((char*)&m_sampleMask, sizeof(int32_t));
        return true;
    }

}