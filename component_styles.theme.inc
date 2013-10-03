<?php

function _component_styles_class_helper($classes, $ignore = NULL) {
  if (!is_array($classes)) {
    $classes = array($classes);
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
 * render array:
 * #classes => array of classes
 * #attach[js] => array of JS files to attach
 * #attach[css] => array of CSS files to attach
 * #children => child elements
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
