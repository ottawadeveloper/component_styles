<?php
/**
 * @file
 * Contains default theme implementations.
 */

/**
 * Helper function for theme_component_styles_widget() that
 * knows how to take a classes array and an ignore classes array
 * and assemble them into a complete array of classes. Also 
 * deduplicates the classnames.
 * 
 * @param array $classes
 *   An array of classnames that should be present.
 * 
 * @param array $ignore
 *   An array of classnames that should not be present, even if
 *   they are in $classes.
 * 
 * @return array
 *   The filtered array of class names.
 */
function _component_styles_class_helper($classes, $ignore = NULL) {
  if (!is_array($classes)) {
    $classes = array($classes);
  }
  else {
    $classes = array_unique($classes);
  }
  if (empty($ignore)) { return $classes; }
  if (!is_array($ignore)) {
    $ignore = array($ignore);
  }
  $actual = array();
  foreach ($classes as $class) {
    if (!in_array($class, $ignore)) {
      $actual[] = $class;
    }
  }
  return $actual;
}

/**
 * Default theme callback for a component styles widget.
 * 
 * This should return up to three nested containers with the
 * content inside:
 * 
 * parent wrapper:
 *   container:
 *     content wrapper:
 * 
 * @param array $vars
 *   The component styles variables:
 *   - component_styles_widget
 *     - #tag: The tag to use to display.
 *     - #classes: An array of classes for the container.
 *     - #ignore_classes: An array of classes to ignore on the 
 *       container.
 *     - #attach:
 *       - js: An array of JS files to attach.
 *       - css: An array of CSS files to attach.
 *     - #content_wrapper: Will be not empty if a content wrapper
 *       was requested.
 *     - #wrapper_classes: Classes for the content wrapper.
 *     - #wrapper_ignore_classes: Classes that must not be on the
 *       content wrapper.
 *     - #wrapper_tag: The tag to use for the content wrapper.
 *     - #parent_wrapper: Will be not empty if a parent wrapper
 *       was requested.
 *     - #parent_classes: Classes for the parent wrapper.
 *     - #parent_ignore_classes: Classes that must not be on the
 *       parent wrapper.
 *     - #parent_tag: The tag to use for the parent wrapper.
 *     - #children: Inner content for the container/content wrapper.
 * 
 * @return string
 *   HTML as returned by theme_html_tag().
 * 
 * @todo Perhaps this should be broken into three different theme
 *   functions?
 * 
 * @ingroup themeable
 */
function theme_component_styles_widget($vars) {
  $element = $vars['component_styles_widget'];
  $classes = array();
  if (!empty($element['#classes'])) {
    $ignore = !empty($element['#ignore_classes']) ? $element['#ignore_classes'] : array();
    $classes = _component_styles_class_helper($element['#classes'], $ignore);
  }
  $classes[] = 'cs-widget';
  if (!empty($element['#attach'])) {
    if (!empty($element['#attach']['js'])) {
      if (!is_array($element['#attach']['js'])) {
        drupal_add_js($element['#attach']['js']);
      } else {
        foreach ($element['#attach']['js'] as $js) {
          drupal_add_js($js);
        } 
      }
    }
    if (!empty($element['#attach']['css'])) {
      if (!is_array($element['#attach']['css'])) {
        drupal_add_css($element['#attach']['css']);
      } else {
        foreach ($element['#attach']['css'] as $css) {
          drupal_add_css($css);
        } 
      }
    }
  }
  if (!empty($element['#content_wrapper'])) {
    $wrapper_classes = array();
    if (!empty($element['#wrapper_classes'])) {
      $ignore = empty($element['#wrapper_ignore_classes']) ? array() : 
          $element['#wrapper_ignore_classes'];
      $wrapper_classes = _component_styles_class_helper($element['#wrapper_classes'], $ignore);
    }
    $wrapper_classes[] = 'cs-content-wrapper';
    $content = $element['content'];
    $element['content'] = array(
      '#type' => 'html_tag',
      '#tag' => !empty($element['#wrapper_tag']) ? $element['#wrapper_tag'] : 'div',
      '#attributes' => array(
        'class' => implode(' ', $wrapper_classes),
      ),
      '#value' => drupal_render($content),
    );
  }
  $html = theme('html_tag', array(
    'element' => array(
      '#tag' => !empty($element['#tag']) ? $element['#tag'] : 'div',
      '#attributes' => array(
        'class' => implode(' ', $classes),
      ),
      '#value' => drupal_render_children($element),
    ),
  ));
  if (!empty($element['#parent_wrapper'])) {
    $parent_classes = array();
    if (!empty($element['#parent_classes'])) {
      $ignore = empty($element['#parent_ignore_classes']) ? array() : $element['#parent_ignore_classes'];
      $parent_classes = _component_styles_class_helper($element['#parent_classes'], $ignore);
    }
    $parent_classes[] = 'cs-parent-wrapper';
    return theme('html_tag', array(
      'element' => array(
        '#value' => $html,
        '#tag' => !empty($element['#parent_tag']) ? $element['#parent_tag'] : 'div',
        '#type' => 'html_tag',
        '#attributes' => array(
          'class' => implode(' ', $parent_classes),
        ),
      ),
    ));
  }
  return $html;
}
