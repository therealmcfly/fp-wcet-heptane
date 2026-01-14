
;; call it by emacs -batch <filename> -l ../heptaneLog.el -f wcet_log_update


(defun wcet_log_update ()
  (while (search-forward "benchmark: " nil t)
    (copyline)
    (if (search-forward "WCET: " nil t)
	(pasteline)
      ()
      )
    )
  (goto-char (point-min))
  (replace-string "| benchmark:" "|")
  (save-buffer)
  )

(defun copyline ()
  (let (beg end)
    (beginning-of-line)
    (setq beg (point))
    (end-of-line)
    ( setq end (point))
    (copy-region-as-kill beg end )
    )
)

(defun pasteline ()
  (end-of-line)
   (while (< (current-column) 20)
     (insert " ")
     )
   (insert " | ")
  (yank)
)


