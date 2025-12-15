// Theme management for the application
(function() {
  // Single theme detection and application
  function setTheme() {
    const htmlRoot = document.getElementById('html-root');
    const favicon = document.getElementById('favicon');
    const isDarkMode = window.matchMedia('(prefers-color-scheme: dark)').matches;
    
    // Set theme class on html element
    htmlRoot.className = isDarkMode ? 'theme-dark' : 'theme-light';
    
    // Update favicon
    favicon.href = isDarkMode ? '/camera_white.png' : '/camera_black.png';
  }

  // Initialize theme when DOM is ready
  function initTheme() {
    if (document.readyState === 'loading') {
      document.addEventListener('DOMContentLoaded', setTheme);
    } else {
      setTheme();
    }
    
    // Listen for theme changes
    window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', setTheme);
  }

  // Start initialization
  initTheme();
})();