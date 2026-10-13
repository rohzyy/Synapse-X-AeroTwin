#pragma once

// The product name, as data rather than as a literal. SynapseX is a Latin
// wordmark in every language: it is the logo, the window title and the name
// in running text alike.

#include "i18n/BeginCatalog.h"

namespace spirula {
namespace i18n {
namespace msg {
namespace brand {

SS_MSG(product,
    EN("SynapseX"),      JA("SynapseX"),
    ZH_HANS("SynapseX"), ZH_HANT("SynapseX"),
    KO("SynapseX"),      DE("SynapseX"),
    FR("SynapseX"),      ES("SynapseX"),
    PT("SynapseX"),      IT("SynapseX"),
    NL("SynapseX"),      RU("SynapseX"),
    TR("SynapseX"));

// The window decoration.
SS_MSG(window_title,
    EN("SynapseX"),      JA("SynapseX"),
    ZH_HANS("SynapseX"), ZH_HANT("SynapseX"),
    KO("SynapseX"),      DE("SynapseX"),
    FR("SynapseX"),      ES("SynapseX"),
    PT("SynapseX"),      IT("SynapseX"),
    NL("SynapseX"),      RU("SynapseX"),
    TR("SynapseX"));

// One line, under the wordmark on the home screen and in About.
SS_MSG(tagline,
    EN("Reconstruct 3D scenes from photos with Gaussian splatting."),
    JA("写真からガウススプラッティングで3Dシーンを再構成します。"),
    ZH_HANS("用高斯泼溅从照片重建三维场景。"),
    ZH_HANT("以高斯潑濺從相片重建三維場景。"),
    KO("사진에서 가우시안 스플래팅으로 3D 장면을 재구성합니다."),
    DE("Rekonstruiert 3D-Szenen aus Fotos mit Gaussian Splatting."),
    FR("Reconstruit des scènes 3D à partir de photos par Gaussian splatting."),
    ES("Reconstruye escenas 3D a partir de fotos con Gaussian splatting."),
    PT("Reconstrói cenas 3D a partir de fotos com Gaussian splatting."),
    IT("Ricostruisce scene 3D da fotografie con il Gaussian splatting."),
    NL("Reconstrueert 3D-scènes uit foto's met Gaussian splatting."),
    RU("Восстанавливает трёхмерные сцены из фотографий методом гауссова сплаттинга."),
    TR("Fotoğraflardan Gaussian splatting ile 3B sahneler oluşturur."));

SS_MSG(about_line,
    EN("Trains 3D Gaussian Splatting models."),
    JA("3Dガウススプラッティングのモデルを学習します。"),
    ZH_HANS("训练三维高斯泼溅模型。"),
    ZH_HANT("訓練三維高斯潑濺模型。"),
    KO("3D 가우시안 스플래팅 모델을 학습합니다."),
    DE("Trainiert 3D-Gaussian-Splatting-Modelle."),
    FR("Entraîne des modèles 3D Gaussian Splatting."),
    ES("Entrena modelos de 3D Gaussian Splatting."),
    PT("Treina modelos de 3D Gaussian Splatting."),
    IT("Addestra modelli 3D Gaussian Splatting."),
    NL("Traint 3D Gaussian Splatting-modellen."),
    RU("Обучает модели 3D Gaussian Splatting."),
    TR("3D Gaussian Splatting modelleri eğitir."));

}  // namespace brand
}  // namespace msg
}  // namespace i18n
}  // namespace spirula

#include "i18n/EndCatalog.h"
