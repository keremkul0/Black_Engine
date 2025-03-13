// IBLManager.h
#pragma once

#include <memory>

#include "Cubemap.h"
#include <memory>

#include "Engine/Render/Shader/Shader.h"

class IBLManager {
public:
    IBLManager();
    ~IBLManager();

    // Generate prefiltered environment map for IBL
    void GenerateEnvironmentMap(const std::string& hdrPath, int resolution = 512);

    // Get irradiance, prefiltered, and BRDF LUT maps
    std::shared_ptr<Cubemap> GetIrradianceMap() const { return m_IrradianceMap; }
    std::shared_ptr<Cubemap> GetPrefilteredMap() const { return m_PrefilteredMap; }
    unsigned int GetBRDFLutTexture() const { return m_BRDFLutTexture; }

    void BindMaps(unsigned int irradianceUnit = 0,
                 unsigned int prefilteredUnit = 1,
                 unsigned int brdfLutUnit = 2) const;

    void BindTextures(std::shared_ptr<Shader> shader, int &startTextureUnit);

private:
    void GenerateIrradianceMap(const std::shared_ptr<Cubemap>& envMap);
    void GeneratePrefilteredEnvMap(const std::shared_ptr<Cubemap>& envMap);
    void GenerateBRDFLookUpTexture();

    std::shared_ptr<Cubemap> m_EnvironmentMap;
    std::shared_ptr<Cubemap> m_IrradianceMap;
    std::shared_ptr<Cubemap> m_PrefilteredMap;
    unsigned int m_BRDFLutTexture = 0;
};
