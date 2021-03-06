#include "Renderer.hpp"

#include "BulletRenderer.hpp"
#include "ShadowmapRenderer.hpp"

Renderer::Renderer():
        shadowRenderer( std::make_unique<ShadowmapRenderer>() ),
        bulletRenderer( std::make_unique<BulletRenderer>() )
{}
Renderer::Renderer(Renderer&&) = default;
Renderer& Renderer::operator=(Renderer&&) = default;
Renderer::~Renderer() = default;

void Renderer::drawScene(const glm::mat4& camera, const Scene& scene) {
        glm::vec3 cameraPos { glm::inverse(camera) * glm::vec4(0,0,0,1) };
        glm::vec3 shadowCenter {0,0,0};

        shadowRenderer->drawShadowmaps(shadowCenter, scene);

        renderProgram.use();
        renderProgram.uniform("camera", camera);
        renderProgram.uniform("cameraPos", cameraPos);
        renderProgram.uniform("lightDirection", scene.sunDirection);

        glActiveTexture(GL_TEXTURE4);
        auto& shadowmaps = shadowRenderer->shadowmaps;
        shadowmaps.shadowTex.bind();
        renderProgram.uniform("shadowTransform", shadowmaps.projView(0, shadowCenter, scene.sunDirection));
        renderProgram.uniform<int>("numShadowLevels", shadowmaps.numShadowmaps());

        GL::Framebuffer::unbind();

        auto windowSize = glfwWindow.getSize();
        glViewport(0, 0, windowSize.x, windowSize.y);

        renderProgram.uniform("projection", glm::perspectiveFovRH(3.14159f/4.0f, windowSize.x, windowSize.y, 0.01f, 128.0f));

        const glm::vec3& bg = scene.backgroundColor;
        glClearColor(bg.r, bg.g, bg.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderProgram.uniform("sceneAmbient", scene.sceneAmbient);
        renderProgram.uniform("sceneDiffuse", scene.sceneDiffuse);
        renderProgram.uniform("sceneSpecular", scene.sceneSpecular);

        for (auto& obj : scene.objects) {
                drawObject(*obj);
        }

        for (auto& bulletSystem : scene.bulletSystems) {
                bulletSystem->prepareForDraw();
                bulletRenderer->drawBullets(camera, bulletSystem->numBullets(), bulletSystem->getBuffer(), *bulletSystem->img, bulletSystem->getScale());
        }

        glfwWindow.swapBuffers();
}
