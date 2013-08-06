<?php

/**
 * Retrieves a list of information on how to build the form for
 * the component style.
 */
function hook_component_styles() {
  return array(
    'padding_settings' => array(
      'title' => t('Padding'),
      'weight' => 0,
      'include' => array(
        'ext' => 'inc',
        'module' => 'mymodule',
        'name' => 'mymodule.cs.padding',
      ),
      'callbacks' => array(
        'form_simple' => 'mymodule_cs_simple_form_padding',
        'form_advanced' => 'mymodule_cs_advanced_form_padding',
        'preprocess' => 'mymodule_cs_preprocess_padding',
        'render_simple' => 'mymodule_cs_simple_render_padding',
        'render_advanced' => 'mymodule_cs_advanced_render_padding',
        'postprocess' => 'mymodule_cs_postprocess_padding',
      ),
    ),
  );
}