/* EpilogueOS Version 0.0.1
   Sei Satzparad and Paul Merrill - 2022

   Licensed under the MIT license: https://opensource.org/licenses/MIT
*/

// Lisp Library

const char LispLibrary[] PROGMEM =
    ""
    "(defun ps (&rest args) "
    "  (let ((pstemp (pslist))) "
    "    (if (zerop (length args)) "
    "      pstemp "
    "      (dolist (pn pstemp) "
    "        (if (stringp (first args)) "
    "          (if (string= (third pn) (first args)) "
    "            (return pn))) "
    "        (if (numberp (first args)) "
    "          (if (= (first pn) (first args)) "
    "            (return pn))))))) "
    " "
    "(defun thispsid () "
    "  (first (first (thisps)))) ";
