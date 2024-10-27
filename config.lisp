(nyxt:define-nyxt-user-system-and-load "nyxt-user/invader-proxy"
  :depends-on ("invader"))

(define-configuration buffer
  ((default-modes
    (pushnew 'nyxt/mode/vi:vi-normal-mode %slot-value%))))

(define-configuration browser
  ((theme theme:+dark-theme+)))

