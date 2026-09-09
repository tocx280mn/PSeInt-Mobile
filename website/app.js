(() => {
  const root = document.documentElement;
  const menuToggle = document.querySelector('.menu-toggle');
  const navigation = document.querySelector('.site-navigation');
  const themeToggle = document.querySelector('#theme-toggle');
  const lightbox = document.querySelector('#lightbox');
  const lightboxImage = document.querySelector('#lightbox-image');
  const lightboxTitle = document.querySelector('#lightbox-title');

  const setThemeLabel = () => {
    const dark = root.dataset.theme === 'dark';
    themeToggle?.setAttribute('aria-label', dark ? 'Activar tema claro' : 'Activar tema oscuro');
    themeToggle?.setAttribute('title', dark ? 'Activar tema claro' : 'Activar tema oscuro');
  };

  try {
    if (localStorage.getItem('pseint-mobile-theme') === 'dark') root.dataset.theme = 'dark';
  } catch (_) {
    // La web sigue funcionando si el navegador bloquea el almacenamiento local.
  }
  setThemeLabel();

  themeToggle?.addEventListener('click', () => {
    const next = root.dataset.theme === 'dark' ? '' : 'dark';
    if (next) root.dataset.theme = next;
    else delete root.dataset.theme;
    try { localStorage.setItem('pseint-mobile-theme', next); } catch (_) { }
    setThemeLabel();
  });

  menuToggle?.addEventListener('click', () => {
    const open = navigation ? navigation.classList.toggle('is-open') : false;
    menuToggle.setAttribute('aria-expanded', String(open));
  });

  navigation?.querySelectorAll('a').forEach((link) => {
    link.addEventListener('click', () => {
      navigation.classList.remove('is-open');
      menuToggle?.setAttribute('aria-expanded', 'false');
    });
  });

  document.querySelectorAll('[data-lightbox]').forEach((card) => {
    card.addEventListener('click', () => {
      if (!lightbox || !(card instanceof HTMLElement)) return;
      lightboxImage.src = card.dataset.lightbox || '';
      lightboxImage.alt = card.querySelector('img')?.alt || '';
      lightboxTitle.textContent = card.dataset.title || '';
      lightbox.showModal();
    });
  });

  lightbox?.querySelector('.lightbox-close')?.addEventListener('click', () => lightbox.close());
  lightbox?.addEventListener('click', (event) => {
    if (event.target === lightbox) lightbox.close();
  });

  const year = document.querySelector('#year');
  if (year) year.textContent = String(new Date().getFullYear());
})();
