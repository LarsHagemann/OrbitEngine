#include "Object.hpp"
#include "Engine/Component/KeyboardComponent.hpp"

namespace orbit
{

    class DebugObject : public Object
    {
    protected:
        bool _displayDebug;
        std::shared_ptr<KeyboardComponent> _kHandler;
    protected:
        // override this member if you want to extend the debug information
        // for example:
        // void MyDebugObject::ShowDebug() {
        //  DebugObject::ShowDebug();
        //  if (ImGui::Button("debug_button")) { ... }
        // }
        virtual void ShowDebug(Time dt);
        virtual void End();
    public:
        void ShowMatrix(Matrix4f matrix);
        void ShowVector(Vector3f vector);
        void ShowVector(Vector4f vector);
        void ShowQuaternion(Quaternionf quaternion);

        virtual void Init() override;
        virtual void Update(Time dt) override;
    };

}
