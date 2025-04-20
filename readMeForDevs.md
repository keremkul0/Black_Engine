# BlackEngine Log Sistemi Kullanım Rehberi

Bu doküman, BlackEngine log sisteminin nasıl kullanılacağını ve yapılandırılacağını anlatmaktadır.

## Genel Bakış
BlackEngine, `spdlog` tabanlı gelişmiş bir loglama altyapısı kullanır. Log sistemi; farklı kategorilerde ve seviyelerde loglama, spam kontrolü ve asenkron loglama gibi güçlü özellikler sunar.

## Log Seviyeleri
Log seviyeleri önem sırasına göre şöyledir:

- `Trace`: Detaylı hata ayıklama bilgisi.
- `Debug`: Genel hata ayıklama bilgisi.
- `Info`: Bilgilendirme amaçlı mesajlar.
- `Warning`: Uyarı mesajları.
- `Error`: Hata mesajları.
- `Critical`: Kritik hatalar, uygulama çökebilir.

## Loglama Yapmak
Loglama yapmak için hazır makrolar kullanılır:

```cpp
BE_LOG_INFO(KategoriAdi, "Mesaj {}", parametre);
```

### Kategori Tanımlama
Yeni bir log kategorisi tanımlamak için:
```cpp
BE_DEFINE_LOG_CATEGORY(KategoriAdi, "Kategoriİsmi");
```

### Örnek Kullanım
```cpp
BE_DEFINE_LOG_CATEGORY(MainLog, "Main");

int main() {
    BE_LOG_INFO(MainLog, "Uygulama başlatılıyor...");
    BE_LOG_DEBUG(MainLog, "Değişken değeri: {}", degisken);
    if (hataDurumu) {
        BE_LOG_ERROR(MainLog, "Bir hata oluştu!");
    }
    BE_LOG_INFO(MainLog, "Uygulama başarıyla tamamlandı.");
    return 0;
}
```

## Log Seviyelerini Ayarlama
Belirli bir kategoriye log seviyesi vermek için `log_config.json` dosyasını kullanabilirsiniz.

### Örnek Yapılandırma
Kategori ve seviye tanımı:
```json
{
  "categories": {
    "Main": "Info",
    "Renderer": "Debug",
    "Physics": "Warning"
  }
}
```

- Yeni tanımlanan bir kategorinin seviyesini ayarlamak için, yukarıdaki gibi ilgili kategori adını ve seviyesini ekleyin.
- Bu ayar, kategoriye ait hangi seviye ve üzerindeki logların çıktılanacağını belirler.

## Spam Kontrolü
Belirli kategorilerde log spam kontrolünü (rate-limit) etkinleştirmek için:

```json
{
  "spamControl": {
    "Renderer": { "enabled": true, "intervalMs": 3000 },
    "Physics": { "enabled": false, "intervalMs": 0 }
  }
}
```

- `enabled`: Spam kontrolünü aktif veya pasif yapar.
- `intervalMs`: Logların minimum basılma aralığı (milisaniye).

Spam kontrolünü sadece çok sık log basan kategoriler için etkinleştirin.

## Async Yapılandırması
Asenkron loglama ayarları:
```json
{
  "async": {
    "queueSize": 8192,
    "threadCount": 2,
    "overflowPolicy": "overrun_oldest"
  }
}
```
- Bu ayar, loglama işlemlerini arka planda yürütür ve performansı artırır.

## Log Mesaj Formatı
Loglar aşağıdaki formatta basılır:
```
[Tarih Saat] [Seviye] [Kategori] Mesaj (Dosya:Satır)
```

Örnek:
```
[2025-04-18 15:32:10.123] [info] [Engine] Application initialized successfully (Application.cpp:50)
```

## Özet
- Kategori tanımlamak için `BE_DEFINE_LOG_CATEGORY` makrosunu kullanın. Tavsiye kategoriyi dosya başında tanımlamaktır.
- Kategori seviyelerini ve spam kontrolünü `log_config.json` üzerinden yönetin.
- Asenkron loglama ile performansı artırın.

Daha fazla bilgi için ekip sorumlusu ile iletişime geçin.

