<?php
/**
 * @file
 * This is a widget implementation for ctools.
 */

/**
 * Faster way to declare the plugin.
 */
$plugin = array(
  'componentstyle' => array(
    'title' => t('Component Styles'),
    'description' => t('Provides a flexible Panels styles implementation'),
    'render pane' => 'componentstyle_render_pane',
    'render region' => 'componentstyle_render_region',
    'pane settings form' => 'componentstyle_settings_form_pane',
    'settings form' => 'componentstyle_settings_form_region',
  ),
);

/**
 * Retrieves the callback for a given component style.
 * 
 * @param array $style_element
 *   A style element from hook_component_styles().
 * @param string $callback
 *   The callback to retrieve.
 * 
 * @return string
 *   The name of the callback function, or NULL if there is
 *   no callback function.
 */
function component_styles_get_callback(&$style_element, $callback) {
  if (empty($style_element['_file_included'])) {
    if (!empty($style_element['include'])) {
      module_load_include($style_element['include']['ext'],
          $style_element['include']['module'],
          $style_element['include']['name']);
    }
    $style_element['_file_included'] = TRUE;
  }
  if ((!empty($style_element['callbacks'][$callback])) &&
      function_exists($style_element['callbacks'][$callback])) {
    return $style_element['callbacks'][$callback];
  }
  return NULL;
}

/**
 * Ctools content type callback for pane settings.
 */
function componentstyle_settings_form_pane($settings) {
  return _componentstyle_settings_form($settings, COMPONENT_STYLES_TYPE_PANE);
}

/**
 * Ctools content type callback for region settings.
 */
function componentstyle_settings_form_region($settings) {
  return _componentstyle_settings_form($settings, COMPONENT_STYLES_TYPE_REGION);
}

/**
 * Ctools content type callback for settings.
 * 
 * @param string $type
 *   One of the two TYPE constants for component styles.
 */
function _componentstyle_settings_form($settings, $type) {
  $form = array();
  $form['advanced_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#access' => user_access('component styles advanced'),
    '#tree' => TRUE,
    '#weight' => 100,
  );
  $categories = component_styles_get_form_categories($type);
  foreach ($categories as $categoryid => $category) {
    $simple_form = component_styles_get_callback($category, 'form_simple');
    if (!empty($simple_form)) {
      if (empty($settings[$categoryid])) {
        $settings[$categoryid] = array();
      }
      $form[$categoryid] = array(
        '#tree' => TRUE,
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#title' => $category['title'],
        '#weight' => $category['weight'],
      );
      if (!empty($category['description'])) {
        $form[$categoryid]['#description'] = $category['description'];
      }
      $form[$categoryid] += $simple_form($settings[$categoryid], $type);
    }
    $advanced_form = component_styles_get_callback($category, 'form_advanced');
    if (!empty($advanced_form)) {
      if (empty($settings['advanced_settings'][$categoryid])) {
        $settings['advanced_settings'][$categoryid] = array();
      }
      $form['advanced_settings'][$categoryid] = array(
        '#tree' => TRUE,
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#title' => $category['title'],
        '#weight' => $category['weight'],
      );
      if (!empty($category['advanced_description'])) {
        $form['advanced_settings'][$categoryid]['#description'] = $category['advanced_description'];
      }
      $form['advanced_settings'][$categoryid] += $advanced_form($settings['advanced_settings'][$categoryid], $type);
    }
  }
  return $form;
}

/**
 * Generates the default render array for component styles widget.
 * 
 * @param mixed $content
 *   The content from the panels pane/region to style.
 * 
 * @return array
 *   The default render array for that content.
 */
function _component_styles_default_render_array($content) {
  $render = array(
    'cs_render_div' => array(
      '#type' => 'component_styles_widget',
      '#theme' => 'component_styles_widget',
      '#tag' => 'div',
      '#parent_tag' => 'div',
      '#wrapper_tag' => 'div',
      '#content_wrapper' => FALSE,
      '#parent_wrapper' => FALSE,
      '#attach' => array(
        'js' => array(),
        'css' => array(),
      ),
      '#parent_classes' => array(),
      '#parent_ignore_classes' => array(),
      '#classes' => array(),
      '#ignore_classes' => array(),
      '#wrapper_classes' => array(),
      '#wrapper_ignore_classes' => array(),
      'content' => array(),
    ),
  );
  if (!is_array($content)) {
    $render['cs_render_div']['content'] = array(
      '#type' => 'markup',
      '#markup' => $content,
    );
  }
  else {
    $render['cs_render_div']['content'] = $content;
  }
  return $render;
}

/**
 * Applies settings to a render array for a given type.
 * 
 * @param array $content
 *   The render array for the content.
 * @param array $settings
 *   Settings from the configuration form.
 * @param string $type
 *   One of the two type constants.
 */
function _component_styles_apply_settings(&$content, $settings, $type) {
  $categories = component_styles_get_form_categories($type);
  foreach ($categories as $categoryid => $category) {
    $render_settings = array(
      '#type' => $type,
      '#cs_render_simple' => TRUE,
      '#cs_render_advanced' => TRUE,
      '#cs_simple_settings' => isset($settings[$categoryid]) ? 
          $settings[$categoryid] : array(),
      '#cs_advanced_settings' => isset($settings['advanced_settings'][$categoryid]) ? 
          $settings['advanced_settings'][$categoryid] : array(),
      'settings' => $settings,
      'content' => $content,
    );
    
    $preprocess = component_styles_get_callback($category, 'preprocess');
    if (!empty($preprocess)) {
      $preprocess($render_settings, $content);
    }
    
    if ($render_settings['#cs_render_simple']) {
      $simple = component_styles_get_callback($category, 'render_simple');
      if (!empty($simple)) {
        $simple($render_settings['#cs_simple_settings'], $content, $render_settings);
      }
    }
    
    if ($render_settings['#cs_render_advanced']) {
      $advanced = component_styles_get_callback($category, 'render_advanced');
      if (!empty($advanced)) {
        $advanced($render_settings['#cs_advanced_settings'], $content, $render_settings);
      }
    }
    
    $postprocess = component_styles_get_callback($category, 'postprocess');
    if (!empty($postprocess)) {
      $postprocess($render_settings, $content);
    }
  }
}

/**
 * Ctools content type callback to render a region.
 */
function theme_componentstyle_render_region($vars) {
  $content = '';
  foreach ($vars['panes'] as $pane) {
    if (is_array($pane)) {
      $content .= render($pane);
    }
    else {
      $content .= $pane;
    }
  }
  $render = _component_styles_apply_all_settings($content, $vars['settings'], COMPONENT_STYLES_TYPE_REGION);
  return render($render);
}

/**
 * Ctools content type callback to render a pane.
 */
function theme_componentstyle_render_pane($vars) {
  $render = _component_styles_apply_all_settings($vars['content']->content, $vars['settings'], COMPONENT_STYLES_TYPE_PANE);
  return render($render);
}

function _component_styles_apply_all_settings($content, $settings, $type) {
  if (!empty($content['#cs_wrap_children']) && is_array($content['#cs_wrap_children'])) {
    foreach ($content['#cs_wrap_children'] as $index) {
      if (isset($content[$index])) {
        $content[$index] = _component_styles_apply_all_settings($content[$index], $settings, $type);
      }
    }
    return $content;
  }
  $render = _component_styles_default_render_array($content);
  _component_styles_apply_settings($render, $settings, $type);
  return $render;
}
