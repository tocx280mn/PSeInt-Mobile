# Sitio web y descarga de PSeInt Mobile

Esta carpeta contiene el sitio web estático oficial de presentación y descarga de **PSeInt Mobile**. Es un sitio estático puro (HTML5, CSS3 y JavaScript moderno), por lo que no requiere Node.js, PHP ni bases de datos en producción.

## Estructura de archivos para publicación

Para publicar el sitio (en GitHub Pages, Netlify, Cloudflare Pages, Vercel, o cualquier hosting estático), sube el contenido de `website/`:

- `index.html`: Página principal con presentación, capturas interactivas, guía de instalación y descarga del APK.
- `credits.html`: Atribución completa de **Copyleft**, reconocimiento a **Pablo Novara** (creador de PSeInt) y visor íntegro de la licencia **GNU GPL v2**.
- `styles.css`: Estilos visuales con soporte para tema claro y oscuro, diseño adaptable (mobile-first a desktop) y estilos de visor de licencia.
- `app.js`: Interacciones del sitio (alternancia de tema, menú móvil responsivo y lightbox de capturas).
- `site.webmanifest`: Manifiesto para Progressive Web App (PWA).
- `assets/`: Logotipo, iconos de PSeInt y capturas de pantalla de la aplicación.
- `downloads/`: Archivo instalador compilado `PSeInt-Mobile-v1.0.apk`.
- `licenses/`: Archivo de texto plano `pseint-gpl-v2.txt` con la licencia GNU General Public License v2 original.

## Principios de Copyleft y Autoría

PSeInt de escritorio es una obra original concebida y desarrollada por **Pablo Novara** bajo **Copyleft** y la **GNU General Public License v2**. PSeInt Mobile incorpora directamente su motor nativo C++ y mantiene plena compatibilidad con los perfiles y algoritmos. El proyecto móvil respeta rigurosamente esta filosofía: se distribuye libremente, sin copyright comercial privativo, bajo los mismos términos de la GNU GPL v2.

## Prueba local

Puedes probar la web localmente con cualquier servidor HTTP estático. Por ejemplo, desde la carpeta `website/`:

```powershell
python -m http.server 8080
```

Luego abre en tu navegador `http://localhost:8080`.

